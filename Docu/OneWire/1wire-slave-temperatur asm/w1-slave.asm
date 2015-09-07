/**
 * complete 1wire slave implementation
 * 
 * All functions needed for a 1wire slave. You must connect at least one 1wire
 * device to the 1wire master port to obtain an unique id.
 * 
 * ----------------------------------------------------------------------------------
 * 
 * Copyright (C) 2009 runlevel3 GmbH
 * 
 * ----------------------------------------------------------------------------------
 * 
 * LICENSE:
 * 
 * This file is part of AVR-projects.
 * 
 * AVR-projects is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AVR-projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AVR-projects; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * ----------------------------------------------------------------------------------
 * 
 * AVR Assembler 2
 * 
 * ----------------------------------------------------------------------------------
 * 
 * @copyright 2009 runlevel3 GmbH
 * @license   http://www.gnu.org/licenses/gpl.txt GNU General Public License v2
 * @link      http://www.runlevel3.de runlevel3
 **/



;register definitions
.def	w1in_stat	= R18					;state machine
#ifndef	TMP_REGS
#define	TMP_REGS
.def	tmp0		= R24
.def	tmp1		= R25
#endif
#ifndef	INT_REGS
#define	INT_REGS
.def	int_sr		= R15
.def	int_tmp		= R21
#endif

;pin and port definitions
.equ	W1IN		= PB2					;w1in pin = PB2
.equ	W1IN_PORT	= PORTB
.equ	W1IN_DDR	= DDRB
.equ	W1IN_PIN	= PINB

;states for w1 state machine w1in_stat
.equ	W1IN_IDLE	= 0
.equ	W1IN_RESET	= 1
.equ	W1IN_PRESENT= 2
.equ	W1IN_BITSYNC= 3
.equ	W1IN_COMMAND= 4
.equ	W1IN_ACTIVE	= 5
.equ	W1IN_WRITE	= 6

;device commands w1_devcmd
.equ	DC_WRITE_SCRATCH	= 0x0f
.equ	DC_READ_SCRATCH		= 0xaa
.equ	DC_COPY_SCRATCH		= 0x55
.equ	DC_READ_MEM			= 0xf0

;bit definitions for eeprom functions
.equ	EE_BIT_PF	= 5
.equ	EE_BIT_AA	= 7

;reset timer 480us, prescaler = 64, exact -150
.equ	W1T_RESET	= -120
.equ	W1C_RESET	= (1<<CS00)|(1<<CS01)
;presence pause timer 15us, prescaler = 8, exact -38
.equ	W1T_PPAUSE	= -38
.equ	W1C_PPAUSE	= (1<<CS01)
;presence pulse timer 120us, prescaler = 64, exact -38
.equ	W1T_PPULSE	= -38
.equ	W1C_PPULSE	= (1<<CS00)|(1<<CS01)
;sample pulse timer 40us, prescaler = 8, exact -100
.equ	W1T_SPULSE	= -100
.equ	W1C_SPULSE	= (1<<CS01)
;0 pulse timer 40us, prescaler = 8, exact -100
.equ	W1T_0PULSE	= -100
.equ	W1C_0PULSE	= (1<<CS01)

.equ	W1_BUFFER_SIZE	= 32			;32 byte data

.dseg
w1_ramstart:
w1_id:		.byte	8
w1_ptr:		.byte	2
w1_shift:	.byte	1
w1_cmd:		.byte	1
w1_bitcnt:	.byte	1
w1_idchk:	.byte	1
w1_srbit:	.byte	1
w1_devcmd:	.byte	1
w1_numbyte:	.byte	1
w1_ee_ta1:	.byte	1
w1_ee_ta2:	.byte	1
w1_ee_es:	.byte	1
w1_buffer:	.byte	W1_BUFFER_SIZE+1
w1_buffer_end:

.cseg										;Code-Segment


ISR_INT0:
		in		int_sr,SREG
		push	tmp0
		push	tmp1
		sbic	W1IN_PIN,W1IN
		rjmp	int0_high
		sbrc	w1in_stat,W1IN_BITSYNC
		ldi		w1in_stat,1<<W1IN_IDLE		;low slope with BITSYNC should not happen
		sbrc	w1in_stat,W1IN_IDLE
		rjmp	int0_low_idle
		sbrc	w1in_stat,W1IN_RESET
		rjmp	int0_low_reset
		cbr		w1in_stat,1<<W1IN_PRESENT	;start bit I/O
		sbr		w1in_stat,1<<W1IN_BITSYNC	;start bit I/O
		sbrs	w1in_stat,W1IN_WRITE		;write data to w1?
		rjmp	int0_low_nowrite
		sbrs	w1in_stat,W1IN_ACTIVE		;transfer active?
		rjmp	int0_low_write_cmd
		lds		tmp0,w1_shift				;load byte
		rjmp	int0_low_write_ni
int0_low_write_cmd:
		lds		tmp0,w1_idchk				;load actual byte to send
		lds		tmp1,w1_srbit				;load search bit number
		cpi		tmp1,1						;last sub-bit, nothing to write
		breq	int0_low_write_exit
		cpi		tmp1,3						;first sub-bit, send rom-bit
		breq	int0_low_write_ni
		ldi		tmp1,1						;else invert bitmask
		eor		tmp0,tmp1
int0_low_write_ni:
		sbrc	tmp0,0						;output 0?
int0_low_write_exit:
		rjmp	int0_low_nowrite			;no -> reload sample timer -> exit
		sbi		W1IN_DDR,W1IN				;output 0!
		sbr		w1in_stat,1<<W1IN_BITSYNC	;signal 0 output
		rjmp	int0_load_0bit_timer
int0_low_nowrite:
		rjmp	int0_load_sample_timer		;no -> load sample timer
int0_low_reset:								;happens if another device output ppulse faster and INT0 enabled
int0_low_idle:								;inactive before, start timer to measure reset pulse
		rjmp	int0_load_reset_timer

int0_high:
		sbrc	w1in_stat,W1IN_RESET
		rjmp	int0_load_ppause_timer
		clr		tmp0						;stop timer
		out		TCCR0B,tmp0
int0_exit:
		pop		tmp1
		pop		tmp0
		out		SREG,int_sr
		reti

;load timers and enable interrupt or disable INT0
int0_load_ppause_timer:
		ldi		tmp0,W1T_PPAUSE				;load presence pulse pause timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_PPAUSE				;load prescaler
		out		TCCR0B,tmp0
		rjmp	int0_load_timer_dis_int0
int0_load_0bit_timer:
		ldi		tmp0,W1T_0PULSE				;load 0 pulse timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_0PULSE				;load prescaler
		out		TCCR0B,tmp0
		rjmp	int0_load_timer_dis_int0
int0_load_sample_timer:
		ldi		tmp0,W1T_SPULSE				;load sample timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_SPULSE				;load prescaler
		out		TCCR0B,tmp0
		rjmp	int0_load_timer_dis_int0
int0_load_reset_timer:
		ldi		tmp0,W1T_RESET				;load reset timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_RESET				;load prescaler
		out		TCCR0B,tmp0
		rjmp	int0_load_all_timer
int0_load_timer_dis_int0:
		in		tmp0,GIMSK					;disable INT0
		andi	tmp0,~(1<<INT0)
		out		GIMSK,tmp0
int0_load_all_timer:
		ldi		tmp0,1<<TOV0				;clear pending interrupt
		out		TIFR,tmp0
		in		tmp0,TIMSK					;enable timer0 overflow interrupt
		ori		tmp0,1<<TOIE0
		out		TIMSK,tmp0
		rjmp	int0_exit



ISR_OVF0:
		in		int_sr,SREG
		push	tmp0
		push	tmp1
		sbrc	w1in_stat,W1IN_IDLE
		rjmp	ovf0_idle
		sbrc	w1in_stat,W1IN_RESET
		rjmp	ovf0_reset
		sbrc	w1in_stat,W1IN_PRESENT
		rjmp	ovf0_present
		rjmp	ovf0_bitsync
ovf0_idle:
		ldi		w1in_stat,1<<W1IN_RESET		;signal reset pulse
		rjmp	ovf0_exit_stop
ovf0_reset:
		ldi		w1in_stat,1<<W1IN_PRESENT	;signal presence pulse
		sbi		W1IN_DDR,W1IN				;output presence pulse
		ldi		tmp0,W1T_PPULSE				;load presence pulse timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_PPULSE				;load prescaler
		out		TCCR0B,tmp0
		rjmp	ovf0_exit_run
ovf0_present:
		cbi		W1IN_DDR,W1IN				;disable presence pulse
		cbr		w1in_stat,1<<W1IN_PRESENT	;disable signal
		in		tmp0,GIMSK					;enable INT0
		ori		tmp0,1<<INT0
		out		GIMSK,tmp0
		ldi		tmp0,8						;init counter, command and numbytes
		sts		w1_bitcnt,tmp0
		clr		tmp0
		sts		w1_cmd,tmp0
		sts		w1_numbyte,tmp0
		ldi		tmp0,1<<INTF0				;clear pending interrupt
		out		GIFR,tmp0
		rjmp	ovf0_exit
ovf0_bitsync:								;reset timeout or bit0 output occured
		cbi		W1IN_DDR,W1IN				;disable output
		in		tmp0,GIMSK					;enable INT0
		ori		tmp0,1<<INT0
		out		GIMSK,tmp0
		sbrc	w1in_stat,W1IN_BITSYNC		;reset timeout or I/O?
		rjmp	ovf0_bitsync_running
		ldi		w1in_stat,1<<W1IN_RESET		;reset found -> signal reset pulse
		rjmp	ovf0_exit_stop
ovf0_bitsync_running:
		rcall	w1_bit_sample
ovf0_exit:
		cbr		w1in_stat,1<<W1IN_BITSYNC	;bit I/O end
		sbic	W1IN_PIN,W1IN				;bus = low -> load reset timer and keep running
		rjmp	ovf0_exit_stop
		ldi		tmp0,W1T_RESET				;load reset timer
		out		TCNT0,tmp0
		ldi		tmp0,W1C_RESET				;load prescaler
		out		TCCR0B,tmp0
		rjmp	ovf0_exit_run
ovf0_exit_stop:
		clr		tmp0
		out		TCCR0B,tmp0					;stop timer
ovf0_exit_run:
		pop		tmp1
		pop		tmp0
		out		SREG,int_sr
		reti

;sample bit and move it to shift register
w1_bit_sample:
		lds		tmp0,w1_ee_es				;set signal incomplete byte
		sbr		tmp0,1<<EE_BIT_PF
		sts		w1_ee_es,tmp0
		sbis	W1IN_PIN,W1IN				;bus = low -> bit 0
		rjmp	w1_bit_sample_bit0
		ldi		tmp0,1
		rjmp	w1_bit_sample_bit
w1_bit_sample_bit0:
		clr		tmp0
w1_bit_sample_bit:							;save bit
		ror		tmp0						;move bit to C flag
		lds		tmp0,w1_shift				;load incomplete byte
		ror		tmp0						;store bit
		sts		w1_shift,tmp0
		sbrc	w1in_stat,W1IN_ACTIVE		;data transfer active or?
		rjmp	w1_bit_sample_active
		sbrc	w1in_stat,W1IN_COMMAND		;command sequence?
		rcall	w1_sample_cmd
w1_bit_sample_active:
		lds		tmp0,w1_bitcnt				;dec bit counter
		dec		tmp0
		sts		w1_bitcnt,tmp0				;store counter
		brne	w1_bit_sample_exit
		ldi		tmp0,8						;init bit counter
		sts		w1_bitcnt,tmp0				;store counter
		rcall	w1_byte_ready
w1_bit_sample_exit:
		ret

;do match or search rom commands
w1_sample_cmd:
		lds		tmp0,w1_cmd					;load command
		cpi		tmp0,MATCH_ROM
		breq	w1_sample_cmd_match
		lds		tmp0,w1_srbit				;load search bit number
		cpi		tmp0,1						;last sub-bit, read rom-bit, match or end
		brne	w1_sample_cmd_exit_search
w1_sample_cmd_match:
		lds		tmp0,w1_shift				;restore received bit
		rol		tmp0						;move to bit 0
		rol		tmp0						;move to bit 0
		lds		tmp1,w1_idchk				;load actual byte to check
		eor		tmp0,tmp1					;check if lowest bit equal to received one
		andi	tmp0,1						;check only 1 bit
		brne	w1_sample_cmd_match_err		;not equal -> no rom match
		ror		tmp1						;move bitn -> bitn-1
		sts		w1_idchk,tmp1				;store actual byte to check
		lds		tmp0,w1_cmd					;load command
		cpi		tmp0,MATCH_ROM
		breq	w1_sample_cmd_exit
		rjmp	w1_sample_cmd_exit_search
w1_sample_cmd_match_err:
		ldi		w1in_stat,1<<W1IN_IDLE		;no rom match -> idle state
		ret
w1_sample_cmd_exit_search:
		lds		tmp1,w1_bitcnt				;dec w1_bitcnt only every 3rd received bit
		inc		tmp1
		sts		w1_bitcnt,tmp1
		lds		tmp0,w1_srbit				;load search bit number
		dec		tmp0
		sts		w1_srbit,tmp0				;store search bit number
		brne	w1_sample_cmd_exit
		ldi		tmp0,3						;init search sub-bit
		sts		w1_srbit,tmp0				;store search bit number
		dec		tmp1						;3rd cycle -> change w1_bitcnt
		sts		w1_bitcnt,tmp1
w1_sample_cmd_exit:
		ret

;process received byte from shift register
w1_byte_ready:
		push	XL
		push	XH
		lds		tmp0,w1_ee_es				;set signal complete byte
		cbr		tmp0,1<<EE_BIT_PF
		sts		w1_ee_es,tmp0
		lds		XL,w1_ptr					;load data pointer
		lds		XH,w1_ptr+1
		sbrc	w1in_stat,W1IN_ACTIVE		;receive/transmit data active?
		rjmp	w1_byte_active
		lds		tmp0,w1_numbyte				;sequence byte number
		cpi		tmp0,0
		breq	w1_byte_0					;first byte -> rom command
		rjmp	w1_romcmd_matchsearch		;else (not yet active) -> match/search rom
w1_byte_0:
		lds		tmp0,w1_numbyte				;sequence byte number inc
		inc		tmp0
		sts		w1_numbyte,tmp0
		lds		tmp0,w1_shift				;load received byte
		sts		w1_cmd,tmp0					;-> rom command
		sbr		w1in_stat,1<<W1IN_COMMAND	;command receive cycle complete
		cpi		tmp0,SKIP_ROM
		breq	w1_romcmd_skip_init
		cpi		tmp0,MATCH_ROM
		breq	w1_romcmd_match_init
		cpi		tmp0,SEARCH_ROM
		breq	w1_romcmd_search_init
		ldi		w1in_stat,1<<W1IN_IDLE		;rom command unknown, idle state
		rjmp	w1_byte_ready_exit
w1_romcmd_skip_init:
		sbr		w1in_stat,1<<W1IN_ACTIVE	;data transfer active
		rjmp	w1_byte_ready_exit_ptr_ld
w1_romcmd_search_init:
		ldi		tmp0,3						;init search bit number
		sts		w1_srbit,tmp0
		sbr		w1in_stat,1<<W1IN_WRITE		;signal write data to w1
w1_romcmd_match_init:
		ldi		XL,LOW(w1_id)				;init data pointer to id store
		ldi		XH,HIGH(w1_id)
w1_romcmd_matchsearch:
		cpi		XL,LOW(w1_id+8)				;last id byte received
		brne	w1_romcmd_matchsearch_cont	;no continue
		lds		tmp0,w1_cmd					;load rom command
		cpi		tmp0,MATCH_ROM
		brne	w1_romcmd_search_end
		sbr		w1in_stat,1<<W1IN_ACTIVE	;match rom end -> data transfer active
		rjmp	w1_byte_ready_exit_ptr_ld
w1_romcmd_search_end:
		ldi		w1in_stat,1<<W1IN_IDLE		;search rom end -> idle state
		rjmp	w1_byte_ready_exit_ptr
w1_romcmd_matchsearch_cont:
		ld		tmp0,X+
		sts		w1_idchk,tmp0
		rjmp	w1_byte_ready_exit_ptr		;do not inc sequence number
w1_byte_active:
		lds		tmp0,w1_numbyte				;sequence byte number
		cpi		tmp0,1
		brne	w1_byte_active_data			;second byte -> device command
		lds		tmp0,w1_shift
		sts		w1_devcmd,tmp0				;store device command
		clr		tmp0						;init crc16
		sts		crc16_data_h,tmp0
		sts		crc16_data_l,tmp0
		lds		tmp0,w1_devcmd
		rcall	crc16
		lds		tmp0,w1_devcmd
		cpi		tmp0,DC_READ_SCRATCH
		brne	w1_byte_active_data_other
		lds		tmp0,w1_numbyte				;sequence byte number extra inc
		inc		tmp0
		sts		w1_numbyte,tmp0
		rjmp	w1_data_read_scratch_tmp	;set up first byte to send
w1_byte_active_data:
		lds		tmp0,w1_devcmd				;load active device command
		cpi		tmp0,DC_WRITE_SCRATCH
		breq	w1_data_write_scratch
		cpi		tmp0,DC_READ_SCRATCH
		breq	w1_data_read_scratch_tmp
		cpi		tmp0,DC_COPY_SCRATCH
		breq	w1_data_copy_scratch_tmp
		cpi		tmp0,DC_READ_MEM
		breq	w1_data_read_mem_tmp
w1_byte_active_data_other:
		rjmp	w1_data_other
w1_byte_ready_exit:
		lds		tmp0,w1_numbyte				;sequence byte number inc
		inc		tmp0
		sts		w1_numbyte,tmp0
		rjmp	w1_byte_ready_exit_ptr
w1_byte_ready_exit_ptr_ld:
		ldi		XL,LOW(w1_buffer)			;init data pointer to data store
		ldi		XH,HIGH(w1_buffer)
w1_byte_ready_exit_ptr:
		sts		w1_ptr,XL					;save data pointer
		sts		w1_ptr+1,XH
		pop		XH
		pop		XL
		ret

w1_data_read_scratch_tmp:
		rjmp	w1_data_read_scratch
w1_data_copy_scratch_tmp:
		rjmp	w1_data_copy_scratch
w1_data_read_mem_tmp:
		rjmp	w1_data_read_mem


;write scratchpad TA1 + TA2 + data, at the end send crc16
w1_data_write_scratch:
		lds		tmp0,w1_numbyte				;sequence byte number, byte 0 = romcmd, byte 1 = devcmd
		cpi		tmp0,2						;byte 2 -> write TA1
		breq	w1_data_write_scratch_ta1
		cpi		tmp0,3						;byte 3 -> write TA2
		breq	w1_data_write_scratch_ta2
		cpi		XL,LOW(w1_buffer_end)		;end address reached?
		brne	w1_data_write_scratch_cont
		lds		tmp0,crc16_data_l			;send high byte crc16
		sts		w1_shift,tmp0
		ser		tmp0						;send 0xff for consecutive reads
		sts		crc16_data_l,tmp0
		rjmp	w1_byte_ready_exit_ptr		;do not inc numbytes
w1_data_write_scratch_cont:
		lds		tmp0,w1_shift				;load data
		st		X+,tmp0
		rcall	crc16						;update crc
		lds		tmp0,w1_numbyte				;sequence byte number
		cpi		tmp0,4						;byte 4 -> do not inc es
		breq	w1_data_write_scratch_nies
		lds		tmp0,w1_ee_es				;update end address
		inc		tmp0
		sts		w1_ee_es,tmp0
w1_data_write_scratch_nies:
		cpi		XL,LOW(w1_buffer_end)		;end address reached?
		brne	w1_data_write_scratch_noend
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		lds		tmp0,crc16_data_h			;send low byte crc16
		sts		w1_shift,tmp0
w1_data_write_scratch_noend:
		rjmp	w1_byte_ready_exit
w1_data_write_scratch_ta1:
		lds		tmp0,w1_shift				;load data
		andi	tmp0,LOW(EEPROMEND)			;check end address
		sts		w1_ee_ta1,tmp0				;save start address low
		rcall	crc16
		rjmp	w1_byte_ready_exit
w1_data_write_scratch_ta2:
		lds		tmp0,w1_shift				;load data
		andi	tmp0,HIGH(EEPROMEND)		;check end address
		sts		w1_ee_ta2,tmp0				;save start address high
		rcall	crc16						;update crc
		lds		tmp0,w1_ee_ta1				;compute scatchpad start address
		andi	tmp0,W1_BUFFER_SIZE-1
		sts		w1_ee_es,tmp0				;store start address
		clr		tmp1						;fuer adc
		add		XL,tmp0
		adc		XH,tmp1						;add carry
		rjmp	w1_byte_ready_exit

;read scratchpad TA1 + TA2 + ES + data
w1_data_read_scratch:
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		lds		tmp0,w1_numbyte				;sequence byte number, byte 0 = romcmd, byte 1 = devcmd
		cpi		tmp0,2						;byte 2 -> send TA1
		breq	w1_data_read_scratch_ta1
		cpi		tmp0,3						;byte 3 -> send TA2
		breq	w1_data_read_scratch_ta2
		cpi		tmp0,4						;byte 4 -> send ES
		breq	w1_data_read_scratch_es
		ld		tmp0,X						;load data
		cpi		XL,LOW(w1_buffer_end)		;end address reached?
		brne	w1_data_read_scratch_cont
		ser		tmp0						;read behind last adrdess -> read 1s
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit_ptr		;do not inc numbytes
w1_data_read_scratch_cont:
		sts		w1_shift,tmp0
		adiw	XL,1						;inc data pointer
		rjmp	w1_byte_ready_exit
w1_data_read_scratch_ta1:
		lds		tmp0,w1_ee_ta1				;load data
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit
w1_data_read_scratch_ta2:
		lds		tmp0,w1_ee_ta2				;load data
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit
w1_data_read_scratch_es:
		lds		tmp0,w1_ee_es				;load data
		sts		w1_shift,tmp0
		lds		tmp0,w1_ee_ta1				;compute scatchpad start address
		andi	tmp0,W1_BUFFER_SIZE-1
		clr		tmp1						;fuer adc
		add		XL,tmp0
		adc		XH,tmp1						;add carry
		rjmp	w1_byte_ready_exit

;copy scratchpad to eeprom, check authorisation TA1 + TA2 + ES
w1_data_copy_scratch:
		lds		tmp1,w1_shift				;check authorisation
		lds		tmp0,w1_numbyte				;sequence byte number, byte 0 = romcmd, byte 1 = devcmd
		cpi		tmp0,2						;byte 2 -> check TA1
		breq	w1_data_copy_scratch_ta1
		cpi		tmp0,3						;byte 3 -> check TA2
		breq	w1_data_copy_scratch_ta2
		cpi		tmp0,4						;byte 4 -> check ES
		breq	w1_data_copy_scratch_es
		ldi		tmp0,0x55					;read more data -> alternating 1s and 0s
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit_ptr
w1_data_copy_scratch_ta1:
		lds		tmp0,w1_ee_ta1
		rjmp	w1_data_copy_scratch_chk
w1_data_copy_scratch_ta2:
		lds		tmp0,w1_ee_ta2
		rjmp	w1_data_copy_scratch_chk
w1_data_copy_scratch_es:
		lds		tmp0,w1_ee_es
w1_data_copy_scratch_chk:
		cpse	tmp0,tmp1
		rjmp	w1_data_copy_scratch_noaa
		lds		tmp0,w1_numbyte				;sequence byte number
		cpi		tmp0,4						;byte 4 -> check success -> write eeprom
		brne	w1_data_copy_scratch_exit
		lds		tmp0,w1_ee_es				;set signal authorisation active
		sbr		tmp0,1<<EE_BIT_AA
		sts		w1_ee_es,tmp0
		push	YL
		push	YH
		lds		XL,w1_ee_ta1				;load eeprom start address
		lds		XH,w1_ee_ta2
		ldi		YL,LOW(w1_buffer)			;load buffer start address
		ldi		YH,HIGH(w1_buffer)
		mov		tmp0,XL						;compute scatchpad start address
		andi	tmp0,W1_BUFFER_SIZE-1
		clr		tmp1						;fuer adc
		add		YL,tmp0
		adc		YH,tmp1						;add carry
		lds		tmp1,w1_ee_es				;compute numbytes
		andi	tmp1,W1_BUFFER_SIZE-1
		sub		tmp1,tmp0
		inc		tmp1
w1_data_copy_scratch_wait:
  		sbic	EECR,EEPE					;wait for previous write
		rjmp	w1_data_copy_scratch_wait
		ldi		tmp0,(0<<EEPM1)|(0<<EEPM0)	;set programming mode
		out		EECR,tmp0
		out		EEARH,XH					;setup write address
		out		EEARL,XL
		adiw	XL,1						;inc data pointer
		ld		tmp0,Y+						;load data
		out		EEDR,tmp0
		sbi		EECR,EEMPE					;eeprom program enable
		sbi		EECR,EEPE					;initiate write
		dec		tmp1
		brne	w1_data_copy_scratch_wait
		pop		YH
		pop		YL
		ldi		tmp0,1<<INTF0				;clear pending interrupt
		out		GIFR,tmp0
		ldi		tmp0,1<<TOV0				;clear pending interrupt
		out		TIFR,tmp0
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		ldi		tmp0,0x55					;read more data -> alternating 1s and 0s
		sts		w1_shift,tmp0
w1_data_copy_scratch_exit:
		rjmp	w1_byte_ready_exit
w1_data_copy_scratch_noaa:
		ldi		w1in_stat,1<<W1IN_IDLE		;authorisation wrong -> idle state
		rjmp	w1_byte_ready_exit_ptr

;read eeprom directly, starting at address TA2:TA1
w1_data_read_mem:
		lds		tmp0,w1_numbyte				;sequence byte number, byte 0 = romcmd, byte 1 = devcmd
		cpi		tmp0,2						;byte 2 -> write TA1
		breq	w1_data_read_mem_ta1
		cpi		tmp0,3						;byte 3 -> write TA2
		breq	w1_data_read_mem_ta2
w1_data_read_mem_data:
		cpi		XL,LOW(EEPROMEND)			;end address reached?
		brne	w1_data_read_mem_wait
		cpi		XH,HIGH(EEPROMEND)			;end address reached?
		brne	w1_data_read_mem_wait
		ser		tmp0
		sts		w1_shift,tmp0				;yes -> send 1s
		rjmp	w1_byte_ready_exit_ptr
w1_data_read_mem_wait:
  		sbic	EECR,EEPE					;wait for previous write
		rjmp	w1_data_read_mem_wait
		out		EEARH,XH					;setup address
		out		EEARL,XL
		sbi		EECR,EERE					;start read -> set EERE
		in		tmp0,EEDR					;read eeprom data
		sts		w1_shift,tmp0
		adiw	XL,1						;inc data pointer
		rjmp	w1_byte_ready_exit_ptr
w1_data_read_mem_ta1:
		lds		XL,w1_shift					;load data
		andi	XL,LOW(EEPROMEND)			;check end address
		rjmp	w1_byte_ready_exit
w1_data_read_mem_ta2:
		lds		XH,w1_shift					;load data
		andi	XH,HIGH(EEPROMEND)			;check end address
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		lds		tmp0,w1_numbyte				;last update sequence byte number
		inc		tmp0
		sts		w1_numbyte,tmp0
		rjmp	w1_data_read_mem_data		;load first byte to shift register

;set chip id, compute crc and store it, input tmp0 = id
w1_id_set_chip:
		sts		w1_id,tmp0					;store chip id
		ldi		YL,LOW(w1_id)
		ldi		YH,HIGH(w1_id)
		ldi		tmp1,7						;compute crc over 7 bytes
		rcall	w1_crc
		lds		tmp0,crc8_data				;load computed value
		sts		w1_id+7,tmp0				;store crc
		ret


w1_slave_init:
		ldi		XL,LOW(w1_ramstart)
		ldi		XH,HIGH(w1_ramstart)
		clr		tmp0
w1_slave_init_ram:
		st		X+,tmp0						;init vars -> clear all ram locations
		cpi		XL,LOW(w1_buffer_end)
		brne	w1_slave_init_ram

		in		tmp0,GIMSK					;global enable INT0
		ori		tmp0,1<<INT0
		out		GIMSK,tmp0
		in		tmp0,MCUCR					;any state change triggers INT0
		ori		tmp0,1<<ISC00
		out		MCUCR,tmp0
		ldi		w1in_stat,1<<W1IN_IDLE		;init state machine
		cbi		W1IN_PORT,W1IN

		sei

		rcall	w1_init						;init w1 master
		rcall	w1_search_all_roms			;search for roms

		mov		tmp0,w1_stat				;any error flag?
		andi	tmp0,~(1<<W1_SB0|1<<W1_SB1)	;except W1_SB0 W1_SB1
		breq	w1_ok


w1_ok:
		ldi		tmp0,CHIP_DS18B20			;rom type 0x28 -> DS18B20
		set									;start new type search
		rcall	w1_find_rom_type
		tst		YL
		brne	w1_id_found
		tst		YH
		brne	w1_id_found
		ldi		tmp0,CHIP_DS18S20			;rom type 0x10 -> DS18S20
		set									;start new type search
		rcall	w1_find_rom_type
		tst		YL
		brne	w1_id_found
		tst		YH
		brne	w1_id_found
		rjmp	w1_id_not_found
w1_id_found:
		ldi		XL,LOW(w1_id)				;init data pointer to id store
		ldi		XH,HIGH(w1_id)
		ldi		tmp1,8						;loop counter
w1_id_copy_loop:
		ld		tmp0,Y+
		st		X+,tmp0
		dec		tmp1
		brne	w1_id_copy_loop
		rjmp	w1_id_ok
w1_id_not_found:
		ldi		ZL,LOW(2*id_default)		;load default id
		ldi		ZH,HIGH(2*id_default)
		ldi		XL,LOW(w1_id)				;init data pointer to id store
		ldi		XH,HIGH(w1_id)
		ldi		tmp1,7						;loop counter
w1_id_not_found_loop:
		lpm		tmp0,Z+						;load default id
		st		X+,tmp0						;and store it
		dec		tmp1
		brne	w1_id_not_found_loop
		lds		tmp0,w1_id					;load chip id
		rcall	w1_id_set_chip				;set chip id, compute and store crc
w1_id_ok:
		ret




id_default:
		.db		0x72,0x6c,0x33,0x02,0x00,0x00,0x00,0x00
str_error:
		.db		"w1-error: ",0,0
str_id:
		.db		"w1-id:",0,0
str_noid:
		.db		"connect w1-id DS2401 to PB4!, using default w1-id:",0,0
str_init:
		.db		"ATTiny45 w1-slave",13,10,0


w1_slave_cseg_end:							;cseg end address

.org	INT0addr
		rjmp	ISR_INT0					;INT0
.org	OVF0addr
		rjmp	ISR_OVF0					;TIM0_OVF
