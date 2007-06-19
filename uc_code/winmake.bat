@echo off
echo **************************************** main.c *****************************************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code main.c
echo **************************************** autofiles\usbdsc.c *****************************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code autofiles\usbdsc.c
echo **************************************** system\usb\usbmmap.c ***************************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code system\usb\usbmmap.c
echo **************************************** system\usb\class\cdc\cdc.c *********************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code system\usb\class\cdc\cdc.c
echo **************************************** system\usb\usb9\usb9.c *************************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code system\usb\usb9\usb9.c
echo **************************************** system\usb\usbctrltrf\usbctrltrf.c *************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code system\usb\usbctrltrf\usbctrltrf.c
echo **************************************** system\usb\usbdrv\usbdrv.c *********************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code system\usb\usbdrv\usbdrv.c
echo **************************************** upp\upp.c **************************************************
sdcc -c -mpic16 -p18f2550 -I c:\usbpicprog\uc_code upp\upp.c
