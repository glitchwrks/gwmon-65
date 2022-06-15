SHELL		:= /bin/bash

AFLAGS		= -t none
LFLAGS		= -t none

CC		= cc65
CA		= ca65
CL		= cl65

all: oms6507sbc osi1 r65x1qsbc

oms6507sbc: smoms6507.hex

smoms6507.hex: smoms6507.bin
	srec_cat smoms6507.bin -binary -offset 0x1400 -fill 0xFF 0x0000 0x1400 -o smoms6507.hex -intel -address-length=2

smoms6507.bin: smoms6507.o
	$(CL) $(LFLAGS) -C oms6507sbc.cfg -o smoms6507.bin smoms6507.o

smoms6507.o: smoms6507.a65
	$(CA) $(AFLAGS) -l smoms6507.lst -o smoms6507.o smoms6507.a65

osi1: smosi1.hex

smosi1.hex: smosi1.bin
	srec_cat smosi1.bin -binary -offset=0x1000 -o smosi1.hex -intel -address-length=2

smosi1.bin: smosi1.o
	$(CL) $(LFLAGS) -o smosi1.bin smosi1.o

smosi1.o: smosi1.a65
	$(CA) $(AFLAGS) -l smosi1.lst -o smosi1.o smosi1.a65

r65x1qsbc: smr6501q.hex

smr6501q.hex: smr6501q.bin
	srec_cat smr6501q.bin -binary -offset=0x1000 -o smr6501q.hex -intel -address-length=2

smr6501q.bin: smr6501q.o
	$(CL) $(LFLAGS) -o smr6501q.bin smr6501q.o

smr6501q.o: smr6501q.a65
	$(CA) $(AFLAGS) -l smr6501q.lst -o smr6501q.o smr6501q.a65

clean:
	rm -f *.o *.bin *.hex *.lst

distclean: clean
