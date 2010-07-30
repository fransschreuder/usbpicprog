##Linux Makefile##
#wine mcc18
CC=wine ~/.wine/drive_c/mcc18/bin/mcc18.exe
LINK=wine mplink 
INSTALL=usbpicprog -e -s -w -f uc_code.hex

#wine mcc18 -I/home/frans/usbpicprog/trunk/uc_code/ -I/home/frans/.wine/drive_c/mcc18/h/ -p 18f2550 -fo=prog_lolvl.o prog_lolvl.c 
#wine mplink /k/ rm18f4550.lkr /l/home/frans/.wine/drive_c/mcc18/lib/ /ouc_code.cof /muc_code.map main.o usbdsc.o interrupt.o usbmmap.o usbgen.o usb9.o usbctrltrf.o usbdrv.o upp.o prog.o prog_lolvl.o 

FLAGS=-p=18f2550 -I=~/.wine/drivve_c/mcc18/h -I~/usbpicprog/trunk/uc_code/
LDFLAGS= /k/ rm18f4550.lkr /l C:\\mcc18\\lib

## Source file suffix ##
SUFFIX=c

## Add source files here ##
## Default is to include all files with suffix ##
SRC=$(wildcard *.$(SUFFIX))

## Object files, created from source names ##
OBJ=$(SRC:.$(SUFFIX)=.o)

## Target executable (final output) ##
TARGET=uc_code.hex

#install:
#	usbpicprog -s -w -f uc_code.hex

all: $(TARGET)

## Cleanup all object and generated files ##
clean:
	rm *.o
	rm *.hex

## Make all nonfile/folder targets phony to prevent conflict ##
.phony: clean all

## Link and make final target ##
$(TARGET): $(OBJ)
	$(LINK) $^ $(LDFLAGS) /o $@
	$(INSTALL)

## Compile source into objects ##
%.o: %.$(SUFFIX)
	$(CC) $(FLAGS) $< -fo=$@
	
