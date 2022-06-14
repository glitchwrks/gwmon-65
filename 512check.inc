;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;512CHECK -- Check Object File Size and Pad
;
;Pad out the monitor to 512 bytes for vector alignment, if
;necessary. Warn if the monitor is larger than 512 bytes.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.IF (512 - (* - JMPTAB) - 6) < 0
	.WARNING "*** MONITOR EXCEEDS 512 BYTES ***"
.ELSE
	.REPEAT	(512 - (* - JMPTAB) - 6)
		.byte $FF
	.ENDREP
.ENDIF