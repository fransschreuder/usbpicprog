##Linux Makefile##
#wine mcc18
#WINEOPTS= WINEDEBUG=-all WINEPREFIX="$(HOME)/.local/share/wineprefixes/mcc18"
CC=env $(WINEOPTS) wine c:\\mcc18\\bin\\mcc18.exe
LINK=env $(WINEOPTS) wine c:\\mcc18\\bin\\mplink.exe 
INSTALL=usbpicprog -e -s -w -f uc_code.hex

FLAGS=-p=18f2550 -ls -I=C:\\mcc18\\h -I~/usbpicprog/trunk/uc_code/
//LDFLAGS= /p 18f2550 /m $(TARGET).map /k/ rm18f4550.lkr /l C:\\mcc18\\lib
#depending on the compiler version you need to specify the target for the linker, use the line you need depending on linker errors
LDFLAGS=/m $(TARGET).map /k/ rm18f4550.lkr /l C:\\mcc18\\lib
## Source file suffix ##
SUFFIX=c

## Add source files here ##
## Default is to include all files with suffix ##
SRC=$(wildcard *.$(SUFFIX))
TESTSRC= test/test.c prog_lolvl.c bulk_erase.c read_code.c read_data.c write_code.c write_data.c write_config_bits.c device.c pic24.c
## Object files, created from source names ##
OBJ=$(SRC:.$(SUFFIX)=.o)

## Target executable (final output) ##
TARGET=uc_code.hex

#install:
#	usbpicprog -s -w -f uc_code.hex

all: clean $(TARGET)

## Cleanup all object and generated files ##
clean:
	rm -f *.o
	rm -f *.hex
	rm -f *.map
	rm -f *.cof
	rm -f *.err
# Create test harness
test: clean
	gcc -Wunused -DTEST -Drom="" -Itest -I. $(TESTSRC) -o runtest
	bash -c 'cd baseline; for i in *; do cmp -s $$i <(../runtest $$i) || echo "$$i differs";done;'
	
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
	
