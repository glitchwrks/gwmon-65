SHELL		:= /bin/bash

AFLAGS		= -t none
LFLAGS		= -t none
RMFLAGS		= -f

CC		= cc65
CA		= ca65
CL		= cl65
RM		= rm

all: gwmon-65.hex smr6501q.hex

gwmon-65.hex: gwmon-65.bin
	srec_cat gwmon-65.bin -binary -offset=0x1000 -o gwmon-65.hex -intel -address-length=2

gwmon-65.bin: gwmon-65.o
	$(CL) $(LFLAGS) -o gwmon-65.bin gwmon-65.o

gwmon-65.o: gwmon-65.a65
	$(CA) $(AFLAGS) -l gwmon-65.lst -o gwmon-65.o gwmon-65.a65

smr6501q.hex: smr6501q.bin
	srec_cat smr6501q.bin -binary -offset=0x1000 -o smr6501q.hex -intel -address-length=2

smr6501q.bin: smr6501q.o
	$(CL) $(LFLAGS) -o smr6501q.bin smr6501q.o

smr6501q.o: smr6501q.a65
	$(CA) $(AFLAGS) -l smr6501q.lst -o smr6501q.o smr6501q.a65

clean:
	$(RM) $(RMFLAGS) *.o *.bin *.hex *.lst

distclean: clean
