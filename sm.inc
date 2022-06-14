;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SM -- GWMON-65 Small Monitor
;
;This is the small monitor for GWMON-65. It includes the
;small command processor and common monitor routines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Zero Page Storage
;
;Set the ZPBASE symbol in the customization file to move
;these variables. Defaults to $F0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.IFNDEF	ZPBASE
ZPBASE		= $F0
.ENDIF

ADRLO		= ZPBASE+$2	;Low address byte from GETADR
ADRHI		= ZPBASE+$3	;High address byte from GETADR
NIBHI		= ZPBASE+$4	;Low nybble for ASCII to hex conversion
ENDLO		= ZPBASE+$5	;Low address byte for two-address commands
ENDHI		= ZPBASE+$6	;High address byte for two-address commands
CKSUM		= ZPBASE+$7	;Checksum storage for Intel HEX loader
CSFLAG		= ZPBASE+$8	;Flag to indicate checksum failure
TEMP		= ZPBASE+$F	;Temporary storage

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SETUP -- Prepare hardware, print monitor sign-on message
;
;Falls through to WSTART.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SETUP:	SEI			;Disable interrupts
	CLD			;Clear decimal arithmetic mode.
	LDX	#$FF		;Set up stack pointer
	TXS

	JSR	IOSET		;Do I/O module setup
	LDY	#SIGNON		;Y = offset to SIGNON string
	JSR	PRTCLS		;Print signon message

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;WSTART -- Warm start the monitor
;
;Falls through to the command processor
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
WSTART:	LDX	#$FF		;Reload stack pointer since
	TXS			;we may have CANCELed

	.include "scp.inc"
	.include "common.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Message Strings
;
;These strings are indexed by the labels following STRNGS.
;These indexes may be fed into PRTSTR, PRTCLS, or PRTERR in
;the Y register.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
STRNGS		= *
SIGNON		= ^(* - STRNGS - 1)
	.byte	"GWMON-65 0.1 S" ,'M' + $80

PROMPT		= * - STRNGS - 1
	.byte	LF, '>' + $80

ERRSTR		= * - STRNGS - 1
	.byte	"ERRO", 'R' + $80
