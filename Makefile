SHELL		:= /bin/bash

AFLAGS		= -t none
LFLAGS		= -t none
RMFLAGS		= -f

CC		= cc65
CA		= ca65
CL		= cl65
RM		= rm

all: osi1 r65x1qsbc

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
	$(RM) $(RMFLAGS) *.o *.bin *.hex *.lst

distclean: clean
