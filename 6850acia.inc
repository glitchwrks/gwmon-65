;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;6850ACIA -- Console I/O Drivers for Motorola 6850 ACIA
;
;Set CTLREG and DATREG in the customization file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IOSET -- I/O specific setup routine
;
;pre: none
;post :console serial channel is initialized
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IOSET:	LDA	#$03
	STA	CTLREG		;Reset ACIA
	LDA	#$15
	STA	CTLREG		;8N1, /16 bitrate
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
CINNE:	LDA	CTLREG		;See if we got an incoming char
	LSR			;Test bit 0
	BCC	CINNE		;Wait for character
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
COUT1:	LDA	CTLREG		;Get UART status
	AND	#$02		;Mask off XMTR Data Reg Empty
	BEQ	COUT1		;Transmitter busy, wait
	PLA			;A = character to print
	PHA			;Back on stack
	AND	#$7F		;Strip high bit
	STA	DATREG		;Send it to UART
	PLA			;A = original character to send
COUT2:	RTS