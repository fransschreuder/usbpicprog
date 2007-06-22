@rem @echo off
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -c application_iface.c
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -c usb_descriptors.c
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -c upp.c
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -c main.c

sdcc -o *.o --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df --no-crt -Wl-s18f4550.lkr,-m -mpic16 -p18f4550 -l../lib/libpuf.lib -llibio18f4550.lib -llibc18f.lib -L"c:\Program files\SDCC\lib"


@rem --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df