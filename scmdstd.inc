;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SCMDSTD -- Small Monitor Standard Commands
;
;This file contains implementations of all standard SM
;commands. The command table is located at the end of the
;file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;HDCMD -- Hex dump command
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HDCMD:	JSR	GETADR		;ADRLO, ADRHI = start address
	TAX			;X = low start address
	LDY	ADRHI		;Y = high start address
	JSR	PRTSPC
	JSR	GETADR		;ADRLO, ADRHI = end address
	STA	ENDLO		;Move them to ENDLO, ENDHI
	LDA	ADRHI
	STA	ENDHI
	STX	ADRLO		;ADRLO, ADRHI = start address
	STY	ADRHI
	JMP	HEXDMP		;Return through HEXDMP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;EDTCMD -- Edit memory command
;
;Note that this routine depends on DMPLOC clearing Y to 0x00
;
;Breaks out to main command loop through GETHEX jump to
;WSTART.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EDTCMD:	JSR	GETADR		;ADRLO, ADRHI = address to open
EDTCM1:	JSR	PRTADR		;Print location address
	JSR	PRTSPC
	JSR	DMPLOC		;Print contents of location
	JSR	PRTSPC
	JSR	CIN		;Get user input
	CMP	#NEXTLOC	;Check for NEXTLOC character
	BEQ	EDTCM2		;Yes, go to next location
	JSR	GETHE2		;No, process as hex input
	STA	(ADRLO), Y
	JSR	PRTSPC
	JSR	DMPLOC		;Print contents of location (verification)
EDTCM2:	INC	ADRLO
	BNE	EDTCM1
	INC	ADRHI
	JMP	EDTCM1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;GOCMD -- Call out to a specified address
;
;The stack is primed with the GWMON-65 warm start address.
;Called program can return control to GWMON-65 through a 
;RTS as long as the stack is not disturbed.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GOCMD:	JSR	GETADR		;ADRLO, ADRHI = address to call
	JMP	(ADRLO)		;Jump indirect to address read

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;LODCMD -- Load an Intel HEX file from console
;
;This loader accepts data with both CR (*NIX) and
;CR,LF (DOS/Windows) terminated lines.
;
;Intel HEX record may be ended with:
;
;   * DATA (0x00) record with length of 0
;   * Any nonzero record type
;
;Note that checksumming relies on the various routines that
;read hex-formatted data from the console clearing the CY
;bit as long as the ASCII coded hex chars are valid. We do
;need to CLC before we ADC the second address byte.
;
;post: Intel HEX file loaded
;post: erorr indication printed if a line contains errors
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LODCMD:	LDA	#$00		;Clear CSFLAG flag
	STA	CSFLAG
LODCM1:	JSR	CRLF
LODCM2:	JSR	CINNE
	CMP	#':'
	BNE	LODCM2		;Wait for start colon
	JSR	COUT		;Print start colon
	JSR	GETHEX		;Get record length
	BEQ	LODCM4		;Length == 0, done

	TAX			;Record length in X
	STA	CKSUM		;Start checksumming in CKSUM

	JSR	GETADR		;ADRLO, ADRHI = 16-bit starting address
	ADC	CKSUM		;Checksum low address
	CLC			;Clear any potential carry
	ADC	ADRHI		;Checksum high address
	STA	CKSUM		;Save CKSUM

	JSR	GETHEX		;Get record type
	BNE	LODCM4		;Not Record Type 00 (DATA), done

	TAY			;Set index to 0
LODCM3:	JSR	GETHEX		;This is the main record processing loop
	STA	(ADRLO), Y	;Store byte
	ADC	CKSUM
	STA	CKSUM		;Checksum data byte
	INY			;Move memory pointer up
	DEX			;Decrement record length counter
	BNE	LODCM3		;Not done with the line
	JSR	GETHEX		;Get checksum byte
	ADC	CKSUM
	BEQ	LODCM1		;Process more records
	LDA	#'*'		;Checksum bad, indicate it
	STA	CSFLAG
	JSR	COUT
	JMP	LODCM1
LODCM4:	JSR	CIN		;Done getting data, eat chars
	CMP	#LF
	BNE	LODCM4		;No LF, keep eating

	LDA	CSFLAG		;Got LF, see if any checksum errors exist
	BEQ	LODCM5		;No, done
	JMP	ERROR		;Yes, tell us about it
LODCM5:	RTS			;Got LF, return to command loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;CMDTAB -- Table/array of commands
;
;Table entry structure:
;    * Single command character, lowercase only
;    * Pointer to implementation routine
;
;The last entry should contain 0x00 for the command char and
;no additional address. It is provided in SCMDNULL.INC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CMDTAB:	.byte	'd'
	.word	HDCMD
	.byte	'e'
	.word	EDTCMD
	.byte	'g'
	.word	GOCMD
	.byte	'l'
	.word	LODCMD
