;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SCP -- GWMON-65 Small Command Processor
;
;This is the small command processor for Glitch Works
;Monitor for 6502 and compatible.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;CMDLP -- Small command processor loop
;
;Get a character from the console device and immediately
;hande it by passing off to helper function.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CMDLP:	LDA	#>WSTART	;Prime stack for RTS
	PHA
	LDA	#<WSTART-1	;Remember to decrement low byte!
	PHA

	LDY	#PROMPT		;Y = offset to PROMPT string
	JSR	PRTCLS		;Print command prompt
	JSR	CIN
	ORA	#$20		;Downcase input

	LDY	#$FF		;Index before start of CMDTAB
CMDLP1:	INY			;Point to next command char
	CMP	CMDTAB, Y
	BEQ	RUNCMD		;Equal, handle it
	LDX	CMDTAB, Y
	BEQ	ERROR		;CMDTAB null terminator, error
	INY			;Not done with CMDTAB, get past the
	INY			;command handler address
	JMP	CMDLP1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ERROR -- Print generic error message and abort
;
;Falls through to PRTERR.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ERROR:	LDY	#ERRSTR		;Y = offset to ERRSTR string

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PRTERR -- Print a null-terminated error string
;
;pre: STRLO, STRHI point to a null-terminated string
;post: string at STRLO, STRHI printed to console
;post: program execution returned to command loop   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRTERR:	JSR	PRTCLS
	JMP	WSTART

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;RUNCMD -- Run a command handler from CMDTAB
;
;pre: Y contains index into CMDTAB to command letter
;post: control transferred to command handler
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RUNCMD:	JSR	PRTSPC
	INY
	LDA	CMDTAB, Y
	STA	ADRLO
	INY
	LDA	CMDTAB, Y
	STA	ADRHI
	JMP	(ADRLO)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;GETADR -- Get a 16-bit address from the console
;
;pre: none
;post: ADRLO and ADRHI contain address from console
;post: A register contains ADRLO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GETADR:	JSR	GETHEX
	STA	ADRHI
	JSR	GETHEX
	STA	ADRLO
	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;GETHEX -- Get byte from console as hex
;
;Enter at GETHE2 with character in A register from EDTCMD.
;
;pre: none
;post: A register contains byte from hex input
;post: Carry flag set if non hex character received
;post: Z flag set if byte is 0x00
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GETHEX:	JSR	CIN		;Get high ASCII nybble
GETHE2:	PHA			;Save high nybble on stack
	JSR	CIN		;Get low ASCII nybble
	STA	NIBLO
	PLA			;A = high nybble
	JSR	HEXBYT
	BCS	GETHE3
	RTS
GETHE3:	JMP	ERROR
