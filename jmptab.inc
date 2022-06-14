;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;JMPTAB -- Jump Table Vectors
;
;These vectors are consistent across all versions of
;GWMON-65
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
JMPTAB:	JMP	SETUP		;Cold start, initializes hardware
	JMP	WSTART		;Warm start
	JMP	COUT		;Output A register to console
	JMP	CIN		;Input A register from console, waits
CSTART:
