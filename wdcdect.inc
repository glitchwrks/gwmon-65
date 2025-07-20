;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;WDCDECT -- Detect buggy WDC 6551 ACIAs
;
;This include provides two routines, which will detect 6551
;ACIAs with a buggy TDRE flag bit. If used directly with a
;GWMON-65 customization, include this file after JMPTAB.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ACISET -- 6551 ACIA setup routine
;
;This routine would normally be part of the GWMON-65 driver,
;but bootstrap routines may require it earlier, so it is
;provided as a separate include.
;
;Falls through to TSTDRE.
;
;pre: ACIAOK variable storage address is defined
;pre: 6522 VIA registers are defined
;pre: 6522 VIA TIMER2 is set up for down count on PB6
;post: ACIAOK is 0 if the TDRE flag is working
;post: ACIAOK is nonzero if the WDC TDRE bug is detected
;post: console serial channel is initialized
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ACISET:	STA	RESET		;Reset the ACIA (data irrelevant)
	LDA	#$0B		;No parity, RX normal, no interrupts
	STA	CMDREG
	LDA	#BITRATE + $10  ;8 bits, 1 stop bit, internal BRG,
	STA	CTLREG		;default bitrate is 9600 bps

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;TSTDRE -- Test 6551 ACIA TDRE functionality
;
;6551 ACIAs produced by WDC have a hardware bug that causes
;the TDRE flag to be stuck set (always 1). This breaks most
;ACIA drivers, as it can't be polled, and also causes a
;continuous stream of interrupts if operating in interrupt
;mode.
;
;We work around this by checking for the buggy TDRE flag,
;and if it is detected, using TIMER2 on a 6522 VIA to count
;ACIA receiver clock transitions, using the TIMER2 underflow
;flag instead of TDRE.
;
;Control is transferred to BTSIGN after testing.
;
;pre: ACIAOK variable storage address is defined
;pre: 6522 VIA registers are defined
;pre: 6522 VIA TIMER2 is set up for down count on PB6
;post: ACIAOK is 0 if the TDRE flag is working
;post: ACIAOK is nonzero if the WDC TDRE bug is detected 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TSTDRE:	LDA	STATREG		;Get UART status
	AND	#$10		;Mask off XMTR Data Reg Empty
	BEQ	TSTDRE		;Transmitter busy, wait
	LDA	#NULL		;Print nothing, test TDRE flag
	STA	DATREG		;Send it to UART
	STA	DATREG		;Send it to UART again
	LDA	STATREG		;Immediately check status
	AND	#$10		;Mask off XMTR Data Reg Empty
	STA	ACIAOK		;Store the results for later
	BEQ	TSTDR1		;TDRE works, done

	LDA	#$B0		;Bit count for T2
	STA	VIAT2CL		;Write to T2 lower latch
	LDA	#0		
	STA	VIAT2CH		;Store it and start timer
TSTDR1:	RTS
