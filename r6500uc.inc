;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;R6500 -- Console I/O Drivers for Rockwell R6500 uCs
;
;This driver uses the onboard serial port found in Rockwell
;R6500 single-chip microcontrollers. The register addresses
;are standardized for the family, so no equates need to be
;set in the customization.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Rockwell 6501/6511 Equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SCCR		= $15		;Serial Communications Control Register
SCSR		= $16		;Serial Communications Status Register
SCDAT		= $17		;Serial Channel Data Register

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IOSET -- I/O specific setup routine
;
;pre: none
;post :console serial channel is initialized
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
CINNE:	LDA	SCSR		;See if we got an incoming char
	LSR			;Test bit 0
	BCC	CINNE		;Wait for character
	LDA	SCDAT		;Load char
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
COUT1:	LDA	SCSR		;Get UART status
	AND	#$40		;Mask off XMTR Data Reg Empty
	BEQ	COUT1		;Transmitter busy, wait
	PLA			;A = character to print
	PHA			;Back on stack
	AND	#$7F		;Strip high bit
	STA	SCDAT		;Send it to UART
	PLA			;A = original character to send
COUT2:	RTS