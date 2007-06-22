/*-------------------------------------------------------------------------
  main.c - docker main function

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

/* $Id: main.cxx,v 1.6 2006/11/11 14:05:16 gaufille Exp $ */

using namespace std;

#include <iostream>
#include <exception>
#include <string>
#include <usb.h>
#include <getopt.h>
#include <signal.h>
#include "config.h"
#include "DataBuffer.h"
#include "HexFile.h"
#include "usbdevice.h"

// device driver - global variable to exit cleanly in the signal handler
static USBDevice* device;

static void help_msg(void)
{
    cout << PACKAGE << " version " << VERSION << endl;
    cout << PACKAGE << " [options] <command>" << endl;
    cout << "Options :\n\t-v vendor_id       : process only devices with this vendor ID" << endl;
    cout <<            "\t-p product_id      : process only devices with this product ID" << endl;
    cout <<            "\t-d device_filename : process only this device" << endl;
    cout <<            "\t-q for quiet operations" << endl;
    cout <<            "\t-h/V displays this message" << endl;
    cout << "Commands :" << endl;
    cout <<             "\tread <file>   : dump the device in <file>" << endl;
    cout <<             "\twrite <file>  : write <file> in the device" << endl;
    cout <<             "\terase         : erase the device (bootloader can not be deleted)" << endl;
    cout <<             "\tverify <file> : verify <file> matches the device memory" << endl;
}

static int usage()
{
    help_msg();
    return -1;
}

static void sighandler(int sig) {
    delete device;
    cerr << "Caught signal " << sig << ".\n";
    exit(-1);
}

int main(int argc, char**argv)
{    
    int    i;
    bool   quiet = false;
    long   product_id   = 0x0000;
    bool   product_flag = false;
    long   vendor_id    = 0x0000;
    bool   vendor_flag  = false;
    string device_id    = "";
    
    // USB busses informations
    struct usb_bus *busses;

    while((i = getopt(argc, argv, "v:p:qVhd:")) > 0)
    {
        switch(i) 
        {
        case 'q':
            quiet = true;
            break;
            
        case 'V':
        case 'h':
            help_msg();
            return 0;
            
        case 'p':
            product_id = strtol(optarg, NULL, 16);
            product_flag = true;
            break;
            
        case 'v':
            vendor_id = strtol(optarg, NULL, 16);
            vendor_flag = true;
            break;
            
        case 'd':
            // The device id is in fact the device filename
            // i.e. something like 001, 023, etc. with linux
            device_id = optarg;
            break;

        default:
            return usage();
        }
    }

    if(optind == argc)
    {
        cerr << argv[0] << "not enough arguments.\n";
        return usage();
    }

    /* Catch some signals to properly shut down the hardware */
    signal(SIGHUP, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGQUIT, sighandler);
    signal(SIGPIPE, sighandler);
    signal(SIGTERM, sighandler);
        
    usb_init();
    usb_find_busses();
    usb_find_devices();
    busses = usb_get_busses();

    for (struct usb_bus *bus = busses; bus; bus = bus->next)
    {
        struct usb_device *dev;

        for (dev = bus->devices; dev; dev = dev->next)
        {
            /* Look for matching devices */
            if ((!vendor_flag  || (dev->descriptor.idVendor  == vendor_id)) && 
                (!product_flag || (dev->descriptor.idProduct == product_id))) 
            {
                if((device_id == "") || (device_id == dev->filename))
                {
                    // Got one !
                    if(!quiet)
                    {
                        cout << "Processing device " << dev->filename << endl;
                    }
                    
                    try 
                    {
                        device = new USBDevice(dev);
                        
                        try 
                        {
                            if(!strcmp(argv[optind], "read"))
                            {
                                DataBuffer buf(8);
                                HexFile *hf;
                        
                                optind++;
                                if(optind == argc)
                                {
                                    cerr << argv[0] << "not enough arguments.\n";
                                    return usage();
                                }
                        
                                device->read(buf);
                                
                                try 
                                {
                                    /* Open the hex file */
                                    hf = new HexFile_ihx8(argv[optind]);
                                } 
                                catch(std::exception& e) 
                                {
                                    cerr << argv[0] << " : " << e.what();
                                    return -1;
                                }
                            
                                try 
                                {
                                    /* Get the device memory map so we know what parts of the buffer
                                     * are valid and save those parts to the hex file. */
                                    for(USBDevice::RangeVector::iterator n = device->device_mmap.begin(); n != device->device_mmap.end(); n++)
                                    {            
                                        if((*n)->flags & FLASH_SECTION_READ)
                                        {
                                            hf->write(buf, (*n)->begin, ((*n)->end - (*n)->begin) + 1);
                                        }
                                    }
                                } 
                                catch(std::exception& e) 
                                {
                                    delete hf;
                                    cerr << argv[0] << " : " << e.what();
                                    return 1;
                                }
                                delete hf;
                            }
                            else if(!strcmp(argv[optind], "write"))
                            {
                                optind++;
                                if(optind == argc)
                                {
                                    cerr << argv[0] << "not enough arguments.\n";
                                    return usage();
                                }
                                
                                // Load the HEX file
                                DataBuffer buf(8);
                                try
                                {
                                    /* Read the hex file into the data buffer */
                                    HexFile *hf = HexFile::load(argv[optind]);
                            
                                    hf->read(buf);
                                    delete hf;
                                }
                                catch(std::exception& e)
                                {
                                    cerr << argv[optind] << ": " << e.what() << endl;
                                    return -1;
                                }
                                device->erase();
                                device->write(buf);
                                device->reset();
                            }
                            else if(!strcmp(argv[optind], "erase"))
                            {
                                device->erase();
                            }
                            else if(!strcmp(argv[optind], "verify"))
                            {
                                optind++;
                                if(optind == argc)
                                {
                                    cerr << argv[0] << "not enough arguments.\n";
                                    return usage();
                                }
                                
                                // Load the HEX file
                                DataBuffer bufh(8);
                                DataBuffer bufd(8);
                                try
                                {
                                    /* Read the hex file into the data buffer */
                                    HexFile *hf = HexFile::load(argv[optind]);
                            
                                    hf->read(bufh);
                                    delete hf;
                                }
                                catch(std::exception& e)
                                {
                                    cerr << argv[optind] << ": " << e.what() << endl;
                                    return -1;
                                }
                                device->read(bufd);
                                for(USBDevice::RangeVector::iterator n = device->device_mmap.begin(); n != device->device_mmap.end(); n++)
                                {            
                                    if((*n)->flags & FLASH_SECTION_READ)
                                    {
                                        for(long i = (*n)->begin; i<=(*n)->end; i++)
                                        {
                                            if(bufd[i] != bufh[i])
                                            {
                                                cerr << "Bytes differs at " << hex << i << " : " << hex <<
                                                        bufd[i] << " != " << hex << bufh[i] << endl;
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                cerr << "Unknown command" << endl;
                            }
                        
                            if(optind == argc)
                            {
                                cerr << argv[0] << "not enough arguments.\n";
                                return usage();
                            }
                        }
                        catch (char const* msg)
                        {
                            cerr << msg << endl; 
                        }
    
                        delete device;
                    }
                    catch (char const* msg)
                    {
                        cerr << msg << endl; 
                    }
                }
            }
        }
    }
}
