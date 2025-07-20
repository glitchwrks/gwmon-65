;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;INCMD -- Memory-mapped input command
;
;Returns through DMPLOC.
;
;pre: IOPAGE is defined
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
INCMD:	JSR	GETHEX		;Get input address
	STA	ADRLO		;Store it at ADRLO
	LDA	#>IOPAGE	;Get upper byte of I/O page address
	STA	ADRHI		;Store it at ADRHI
	JSR	PRTSPC
	JMP	DMPLOC		;Print the input value

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;OUTCMD -- Memory-mapped output command
;
;pre: IOPAGE is defined
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OUTCMD:	JSR	GETHEX		;Get output address
	STA	ADRLO		;Store it at ADRLO
	LDA	#>IOPAGE	;Get upper byte of I/O page address
	STA	ADRHI		;Store it at ADRHI
	JSR	PRTSPC
	JSR	GETHEX		;Get byte to output
	LDY	#0		;Clear Y
	STA	(ADRLO), Y	;Store output byte indirect
	RTS
