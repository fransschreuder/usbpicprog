##Linux Makefile##
#wine mcc18
CC=wine c:\\mcc18\\bin\\mcc18.exe
LINK=wine c:\\mcc18\\bin\\mplink.exe 
INSTALL=usbpicprog -e -s -w -f uc_code.hex

FLAGS=-p=18f2550 -I=C:\\mcc18\\h -I~/usbpicprog/trunk/uc_code/
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

## Compile source into objects ##
%.o: %.$(SUFFIX)
	$(CC) $(FLAGS) $< -fo=$@
	
install:
	$(INSTALL)
	
