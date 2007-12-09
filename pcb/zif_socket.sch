EESchema Schematic File Version 1
LIBS:power,device,transistors,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,contrib,valves,./usbpicprog.cache
EELAYER 23  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title "noname.sch"
Date "9 dec 2007"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 4300 3250 0    60   ~
PGD
Text Label 4300 3150 0    60   ~
PGC
Text Label 1150 2700 0    60   ~
MCLR
Wire Wire Line
	1500 2700 1150 2700
Wire Wire Line
	3900 3150 4300 3150
Wire Wire Line
	7100 5900 7400 5900
Wire Wire Line
	4850 2850 4450 2850
Wire Wire Line
	6300 2150 6750 2150
Wire Wire Line
	4700 5850 4700 5900
Wire Wire Line
	4700 5900 4750 5900
Wire Wire Line
	7850 4350 7850 4100
Wire Wire Line
	7850 4100 7900 4100
Wire Wire Line
	6350 1950 6350 1850
Wire Wire Line
	6350 1850 6300 1850
Connection ~ 1450 1850
Wire Wire Line
	1500 1850 1450 1850
Connection ~ 1450 5450
Wire Wire Line
	1500 5450 1450 5450
Wire Wire Line
	10300 5300 10550 5300
Wire Wire Line
	7900 2900 7550 2900
Wire Wire Line
	8050 2050 8250 2050
Wire Wire Line
	8100 2300 8100 2250
Wire Wire Line
	8100 2250 8050 2250
Wire Wire Line
	8100 1950 8050 1950
Wire Wire Line
	8050 2150 8250 2150
Wire Wire Line
	8050 1850 8250 1850
Wire Wire Line
	10300 5200 10550 5200
Wire Wire Line
	1500 5350 1450 5350
Wire Wire Line
	1450 5350 1450 5500
Wire Wire Line
	1500 1950 1450 1950
Wire Wire Line
	1450 1950 1450 1800
Wire Wire Line
	4800 1750 4800 1850
Wire Wire Line
	4800 1850 4850 1850
Wire Wire Line
	7850 3850 7850 3900
Wire Wire Line
	7850 3900 7900 3900
Wire Wire Line
	7900 4300 7850 4300
Connection ~ 7850 4300
Wire Wire Line
	4750 7200 4700 7200
Wire Wire Line
	4700 7200 4700 7300
Wire Wire Line
	6300 2550 6750 2550
Wire Wire Line
	7100 6000 7400 6000
Wire Wire Line
	4750 6750 4500 6750
Wire Wire Line
	3900 3250 4300 3250
Text Label 4500 6750 0    60   ~
MCLR
Text Label 7400 6000 0    60   ~
PGC
Text Label 7400 5900 0    60   ~
PGD
Text Label 10550 5300 0    60   ~
PGD
Text Label 10550 5200 0    60   ~
PGC
Text Label 7550 2900 0    60   ~
MCLR
Text Label 4450 2850 0    60   ~
MCLR
Text Label 6750 2550 0    60   ~
PGC
Text Label 6750 2150 0    60   ~
PGD
$Comp
L VSS #PWR01
U 1 1 475BDF5A
P 6350 1950
F 0 "#PWR01" H 6350 1950 30  0001 C C
F 1 "VSS" H 6350 1880 30  0000 C C
	1    6350 1950
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR02
U 1 1 475BDF56
P 4800 1750
F 0 "#PWR02" H 4800 1850 30  0001 C C
F 1 "VDD" H 4800 1860 30  0000 C C
	1    4800 1750
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR03
U 1 1 475BDF4E
P 4700 5850
F 0 "#PWR03" H 4700 5950 30  0001 C C
F 1 "VDD" H 4700 5960 30  0000 C C
	1    4700 5850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR04
U 1 1 475BDF49
P 4700 7300
F 0 "#PWR04" H 4700 7300 30  0001 C C
F 1 "VSS" H 4700 7230 30  0000 C C
	1    4700 7300
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR05
U 1 1 475BDF3C
P 1450 5500
F 0 "#PWR05" H 1450 5500 30  0001 C C
F 1 "VSS" H 1450 5430 30  0000 C C
	1    1450 5500
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR06
U 1 1 475BDF39
P 1450 1800
F 0 "#PWR06" H 1450 1900 30  0001 C C
F 1 "VDD" H 1450 1910 30  0000 C C
	1    1450 1800
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR07
U 1 1 475BDF2E
P 7850 3850
F 0 "#PWR07" H 7850 3950 30  0001 C C
F 1 "VDD" H 7850 3960 30  0000 C C
	1    7850 3850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR08
U 1 1 475BDF29
P 7850 4350
F 0 "#PWR08" H 7850 4350 30  0001 C C
F 1 "VSS" H 7850 4280 30  0000 C C
	1    7850 4350
	1    0    0    -1  
$EndComp
Text Label 8250 1850 0    60   ~
MCLR
Text Label 8250 2050 0    60   ~
PGD
Text Label 8250 2150 0    60   ~
PGC
$Comp
L VDD #PWR09
U 1 1 475BDED7
P 8100 1950
F 0 "#PWR09" H 8100 2050 30  0001 C C
F 1 "VDD" H 8100 2060 30  0000 C C
	1    8100 1950
	0    1    1    0   
$EndComp
$Comp
L VSS #PWR010
U 1 1 475BDECF
P 8100 2300
F 0 "#PWR010" H 8100 2300 30  0001 C C
F 1 "VSS" H 8100 2230 30  0000 C C
	1    8100 2300
	1    0    0    -1  
$EndComp
$Comp
L CONN_5 P1
U 1 1 475BDEC0
P 7650 2050
F 0 "P1" V 7600 2050 50  0000 C C
F 1 "CONN_5" V 7700 2050 50  0000 C C
	1    7650 2050
	-1   0    0    1   
$EndComp
$Comp
L PIC12F675 U2
U 1 1 475BDEA5
P 5600 2350
F 0 "U2" H 5550 2350 60  0000 C C
F 1 "PIC12F675" H 5600 3100 60  0000 C C
	1    5600 2350
	1    0    0    -1  
$EndComp
$Comp
L PIC18F452-/PT U1
U 1 1 475BDE94
P 2700 3650
F 0 "U1" H 2700 5600 60  0000 C C
F 1 "PIC18F452-/PT" H 2700 1700 60  0000 C C
	1    2700 3650
	1    0    0    -1  
$EndComp
$Comp
L PIC18F2550 U4
U 1 1 475BDE63
P 9100 4150
F 0 "U4" H 9625 5550 60  0000 C C
F 1 "PIC18F2550" H 9475 2800 60  0000 C C
	1    9100 4150
	1    0    0    -1  
$EndComp
$Comp
L PIC16F628A U3
U 1 1 475BDE4B
P 5800 6550
F 0 "U3" H 6100 7350 60  0000 C C
F 1 "PIC16F628A" H 6250 5750 60  0000 C C
	1    5800 6550
	1    0    0    -1  
$EndComp
$EndSCHEMATC
