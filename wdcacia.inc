;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;WDCACIA -- Console I/O Drivers for WDC 6551 ACIA
;
;This driver is a universal 6551 ACIA driver, and will work
;with WDC 6551 parts with the TDRE flag bug.
;
;Set ACIABASE and VIABASE in the customization file.
;
;A variable called ACIAOK must be defined, which flags
;whether or not the 6551 has the TDRE bug:
;
; ACIAOK == 0, TDRE operational
; ACIAOK != 0, TDRE bug present
;
;If a buggy ACIA is detected, we use TIMER2 from a 6522 VIA
;as a delay loop. For this to work properly, PB6 of the 6522
;must be connected to the receiver clock output (pin 5) of
;the ACIA.
;
;See WDCDECT for notes about including detection routines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Hardware Equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DATREG		= ACIABASE
RESET		= ACIABASE+1
STATREG		= ACIABASE+1
CMDREG		= ACIABASE+2
CTLREG		= ACIABASE+3

VIAT2CL		= VIABASE+8	;TIMER2 Low-Order Latch/Counter
VIAT2CH		= VIABASE+9	;TIMER2 High-Order Counter
VIAIFR		= VIABASE+13	;Interrupt Flag Register

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Bitrate Equates
;
;A selection of common bitrate options for the 6551 ACIA.
;More options exist, see the 6551 datasheet.
;
;Defaults to 9600 bps. Set BITRATE to choose an alternative. 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BPS19200	= $0F
BPS9600		= $0E
BPS4800		= $0C
BPS2400		= $0A
BPS1200		= $08
BPS300		= $06

.IFNDEF	BITRATE
BITRATE		= BPS9600
.ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IOSET -- I/O specific setup routine
;
;I/O setup must be done separately, since a bootstrap
;routine may need to know about the potential TDRE bug ahead
;of loading GWMON-65.
;
;See WDCDETECT and SMGW6502SBC1.A65 for examples.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IOSET:	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;CINNE -- Get a character from the console, no echo
;
;Blocks until a character is available. Warm starts the
;monitor if the received character is the CANCEL char.
;
;pre: serial UART initialized
;post: A register contains received character
;post: monitor warm started if CANCEL received
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CINNE:	LDA	STATREG		;See if we got an incoming char
	AND	#$08		;Test RDRE bit
	BEQ	CINNE		;Wait for character
	LDA	DATREG		;Load char
	CMP	#CANCEL		;CANCEL ASCII code?
	BNE	COUT4		;No, return using COUT's RTS
	JMP	WSTART		;Yes, warm start	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;CIN -- Get a char from the console and echo
;
;Falls through to COUT.
;
;pre: console device is initialized
;post: received char is in A register
;post: received char is echoed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CIN:	JSR	CINNE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;COUT -- Print a character to the console
;
;Strips parity, preserves A register.
;
;This COUT implementation works around the TDRE flag bug
;present in WDC 6551 ACIAs.
;
;pre: ACIAOK is initialized and valid
;pre: A register contains character to print
;post: character printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
COUT:	PHA			;Save A register

	LDA	ACIAOK		;See if TDRE bug is present
	BEQ	COUT2		;No TDRE bug, regular routine

COUT1:	LDA	VIAIFR		;Get the VIA's interrupt flag register
	AND	#$20		;Mask off T2 timeout bit
	BEQ	COUT1		;Not set, keep waiting
	PLA			;A = character to print
	PHA			;Back on stack
	AND	#$7F		;Strip high bit
	STA	DATREG		;Send it to UART
	LDA	#$B0		;Bit count for T2
	STA	VIAT2CL		;Write to T2 lower latch
	LDA	#0		
	STA	VIAT2CH		;Store it and start timer
	BEQ	COUT3		;Always taken as A = 0

COUT2:	LDA	STATREG		;Get UART status
	AND	#$10		;Mask off XMTR Data Reg Empty
	BEQ	COUT2		;Transmitter busy, wait

	PLA			;A = character to print
	PHA			;Back on stack
	AND	#$7F		;Strip high bit
	STA	DATREG		;Send it to UART
COUT3:	PLA			;A = original character to send
COUT4:	RTS
