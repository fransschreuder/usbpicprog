@rem @echo off
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -I../bootloader -c application_iface.c
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -I../bootloader -c main.c
sdcc -mpic16 --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df -p18f4550 -I../include -I../bootloader -c usb_descriptors.c
sdcc -o *.o --optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df --no-crt -Wl-s18f4550.lkr,-m -mpic16 -p18f4550 -llibio18f4550.lib -llibc18f.lib
