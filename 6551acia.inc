;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;6850ACIA -- Console I/O Drivers for MOS 6551 ACIA
;
;Do note that this driver uses polled I/O and will not work
;with WDC 65C51 ACIAs due to a silicon bug in the WDC part.
;
;Set ACIABASE in the customization file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Hardware Equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DATREG		= ACIABASE
RESET		= ACIABASE + 1
STATREG		= ACIABASE + 1
CMDREG		= ACIABASE + 2
CTLREG		= ACIABASE + 3

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
;pre: none
;post :console serial channel is initialized
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IOSET:	STA	RESET		;Reset the ACIA (data irrelevant)
	LDA	#$0B		;No parity, RX normal, no interrupts
	STA	CMDREG
	LDA	#BITRATE + $10  ;8 bits, 1 stop bit, internal BRG,
	STA	CTLREG		;default bitrate is 9600 bps
	RTS

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
	BNE	COUT2		;No, return using COUT's RTS
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
;pre: A register contains character to print
;post: character printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
COUT:	PHA			;Save A register
COUT1:	LDA	STATREG		;Get UART status
	AND	#$10		;Mask off XMTR Data Reg Empty
	BEQ	COUT1		;Transmitter busy, wait
	PLA			;A = character to print
	PHA			;Back on stack
	AND	#$7F		;Strip high bit
	STA	DATREG		;Send it to UART
	PLA			;A = original character to send
COUT2:	RTS
