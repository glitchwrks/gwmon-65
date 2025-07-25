;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;COMMON -- Functions Common to SM and XM
;
;These functions are used by both the SM and XM versions
;of GWMON-65.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ASCII Equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NULL		= $00
CTRLC		= $03
LF		= $0A
CR		= $0D

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Miscellaneous Equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CANCEL		= CTRLC		;ASCII code to use for cancelling current line
				;Default is CTRL+C
NEXTLOC		= CR		;Keystroke for next location in E
				;Defaults to CR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;HEXBYT -- Convert pair of ASCII characters to byte
;
;Destroys contents of TEMP.
;
;pre: NIBHI contains ASCII coded high nybble
;pre: A regsiter contains ASCII coded low nybble
;post: A register contains converted byte if successful
;post: CY flag cleared on successful conversion
;post: Z flag set if conversion results in 0x00
;post: abort through ERROR if conversion fails
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HEXBYT:	JSR	ASCHEX		;Convert low nybble
	STA	TEMP		;Save it in TEMP
HEXBY1:	LDA	NIBHI
	JSR	ASCHEX		;Convert high nybble
	ASL			;Shift high nybble up
	ASL
	ASL
	ASL
HEXBY2:	ORA	TEMP		;OR in high nybble
HEXBY3:	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ASCHEX -- Convert ASCII character to hex nybble
;
;pre: A register contains ASCII coded nybble
;post: A register contains nybble
;post: CY flag clear if ASCII character is valid hex
;post: abort through ERROR if ASCII character is invalid hex
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ASCHEX:	SEC
	SBC	#'0'		;ASCII to decimal bias
	BCC	ASCHE3		;Error, less than '0', done
ASCHE1:	CMP	#$0A
	BCC	HEXBY3		;0x0 - 0x9, return using HEXBYT's RTS
ASCHE2:	AND	#$5F		;Upcase
	SEC
	SBC	#$07		;ASCII to hex bias
	CMP	#$10
	BCC	HEXBY3		;No error, return using HEXBYT's RTS
ASCHE3:	JMP	ERROR		;Invalid hex character, abort

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;HEXDMP -- Hex dump memory to console
;
;This routine prints the contents of memory starting at
;ADRLO, ADRHI and ending at ENDLO, ENDHI in 16-byte blocks.
;
;pre: ADRLO, ADRHI contains starting address
;pre: ENDLO, ENDHI register pair contains ending address
;post: contents of memory printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HEXDMP:	LDA	#$00		;Initialize TEMP as completion flag
	STA	TEMP
HEXDM1:	JSR	PRTADR
	LDX	#16		;X = line loop counter
HEXDM2:	JSR	PRTSPC
	JSR	DMPLOC

	LDA	ADRLO		;Compare addresses, see if we just
	CMP	ENDLO		;printed the ending location
	BNE	HEXDM3		;Not done, dump more
	LDA	ADRHI		;Low bytes the same, check high bytes
	CMP	ENDHI
	BNE	HEXDM3		;Not done, dump more
	DEC	TEMP		;TEMP = nonzero, done

HEXDM3:	INC	ADRLO		;Increment the current dump location
	BNE	HEXDM4
	INC	ADRHI
HEXDM4:	DEX			;Decrement line loop counter
	BNE	HEXDM2		;Not zero, print more on this line

	LDA	TEMP		;Get completion flag
	BEQ	HEXDM1		;Haven't hit ENDLO, ENDHI yet, do more
	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTADR -- Print an address to the console
;
;Prints CR, LF, 16-bit address, a space, and a colon. 
;Destroys A register contents.
;
;Returns through COUT.
;
;pre: ADRLO, ADRHI contains address to print
;post: address printed to console as hex
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTADR: JSR	CRLF
	LDA	ADRHI
	JSR	PRTHEX
	LDA	ADRLO
	JSR	PRTHEX
	LDA	#':'
	BNE	PRTSP1		;Branch to COUT thru PRTSPC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DMPLOC -- Print a byte at ADRLO, ADRHI to console
;
;Destroys A and Y register contents.
;
;Falls through to PRTHEX.
;
;pre: ADRLO, ADRHI contains address of byte
;post: byte printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DMPLOC:	LDY	#$00
	LDA	(ADRLO), Y

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTHEX -- Output byte to console as hex
;
;Falls through to PRTNIB.
;
;pre: A register contains byte to be output
;post: byte is output to console as hex
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTHEX: PHA			;Save A register on stack
	LSR			;Rotate high nybble down
	LSR
	LSR
	LSR
	JSR	PRTNIB		;Print high nybble
	PLA			;Restore A register

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTNIB -- Print hex nybble on console
;
;Returns through COUT.
;
;pre: A register contains nybble
;post: nybble printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTNIB:	AND	#$0F
	ORA	#'0'		;Add ASCII bias
	CMP	#':'		;0x0 - 0x9?
	BCC	PRTSP1		;Branch to COUT thru PRTSPC
	ADC	#$06		;Add hex bias
PRTNI1:	BNE	PRTSP1		;Branch to COUT thru PRTSPC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTSPC -- Print a space to the console
;
;Returns through COUT.
;
;pre: none
;post: ASCII space printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTSPC:	LDA	#' '
PRTSP1:	JMP	COUT		;Other routines branch relative here to
				;get to COUT. Saves bytes.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;CRLF -- Print a CR, LF
;
;Returns through COUT.
;
;pre: none
;post: CR, LF printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CRLF:	LDA	#CR
	JSR	COUT
	LDA	#LF
	BNE	PRTSP1		;Branch to COUT thru PRTSPC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTCLS -- Print CR, LF, and a high bit terminated string
;
;Falls through to PRTSTR
;
;pre: Y register contains offset into STRNGS of string to be
;     printed, minus one
;post: CR, LF, and string printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTCLS:	JSR	CRLF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTSTR -- Print a high bit terminated string
;
;Destroys contents of Y register.
;
;pre: Y register contains offset into STRNGS of string to be
;     printed, minus one
;post: string printed to console
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTSTR:	INY			;Point to start of string
	LDA	STRNGS, Y	;A = next char in string
	JSR	COUT
	BPL	PRTSTR		;Done if high bit is set
	RTS
