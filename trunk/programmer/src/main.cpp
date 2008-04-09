/***************************************************************************
 *   Copyright (C) 2008 by Frans Schreuder   *
 *   usbpicprog.sourceforge.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include "read_hexfile.h"
#include "pictype.h"

#include "upp_gui.h"
#include "gtkmm.h"

using namespace std;
using namespace Gtk;

void printHexFile(PicType *picType, ReadHexFile *readHexFile)
{
	int lineSize;
	cout<<"Config Memory"<<endl;
	for(int i=0;i<readHexFile->getConfigMemory().size();i+=16)
	{
		printf("%00000008X::",i+picType->getCurrentPic().ConfigAddress);
		if(i+16<readHexFile->getConfigMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getConfigMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			printf("%02X",readHexFile->getConfigMemory()[i+j]);
		}
		cout<<endl;	
	}
	cout<<"Code Memory"<<endl;
	for(int i=0;i<readHexFile->getCodeMemory().size();i+=16)
	{
		printf("%00000008X::",i);
		if(i+16<readHexFile->getCodeMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getCodeMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			printf("%02X",readHexFile->getCodeMemory()[i+j]);
		}
		cout<<endl;	
	}
	cout<<"Data Memory"<<endl;
	for(int i=0;i<readHexFile->getDataMemory().size();i+=16)
	{
		printf("%00000008X::",i);
		if(i+16<readHexFile->getDataMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getDataMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			printf("%02X",readHexFile->getDataMemory()[i+j]);
		}
		cout<<endl;	
	}
}


int main(int argc, char *argv[])
{
	
	
	if(argc==3)
	{
		PicType *picType;
		ReadHexFile *readHexFile;
		picType=new PicType(argv[2]);
		cout<<picType->getCurrentPic().Name<<endl;
		readHexFile=new ReadHexFile(picType,argv[1]);
		printHexFile(picType,readHexFile);
		
	}
	else if(argc>1)
	{
		cerr<<"\nUsage:\nusbpicprog <Hexfile> <Pictype> \n Or no arguments for GUI"<<endl;
		return EXIT_FAILURE;
	}
	else
	{
		Main kit(argc, argv);
		UppGui uppGui;
		
		Main::run(uppGui);
		
	}
	return EXIT_SUCCESS;
}
