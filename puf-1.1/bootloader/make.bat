@echo off
sdcc -c -mpic16 -p18f2550 application_iface.c
sdcc -c -mpic16 -p18f2550 boot_iface.c
sdcc -c -mpic16 -p18f2550 boot_main.c
sdcc -c -mpic16 -p18f2550 config.c
sdcc -c -mpic16 -p18f2550 ep0.c
sdcc -c -mpic16 -p18f2550 ep1.c
sdcc -c -mpic16 -p18f2550 ep2.c
sdcc -c -mpic16 -p18f2550 flash.c
sdcc -c -mpic16 -p18f2550 interrupt_iface.c
sdcc -c -mpic16 -p18f2550 usb.c
sdcc -c -mpic16 -p18f2550 usb_descriptors.c
gplink *.o pic18f2550.lib libsdcc.lib libc18f.lib -I "c:\Program Files\SDCC\lib\pic16" -o bootloader.hex
