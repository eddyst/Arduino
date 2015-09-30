/**
 * 1wire slave implementation for DS18X20
 * 
 * This program simulates a DS18B20 or DS18S20 connected to the 1wire-master-pin PB4.
 * The goal is, that the temperature is displayed locally and the whole circuit looks
 * like just the sensor to the 1wire bus. So you can replace a sensor with this sensor
 * with local temperature display.
 * 
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



.include "tn45def.inc"

;register definitions
#ifndef	TMP_REGS
#define	TMP_REGS
.def	tmp0		= R24
.def	tmp1		= R25
#endif
#ifndef	TMP2_REGS
#define	TMP2_REGS
.def	tmp2		= R22
.def	tmp3		= R23
#endif

//DS18X20
#define CHIP_DS18B20			0x28
#define CHIP_DS18S20			0x10
//commands
#define DS18X20_CONVERT_T		0x44
#define DS18X20_READ			0xBE
#define DS18X20_WRITE			0x4E
#define DS18X20_EE_WRITE		0x48
#define DS18X20_EE_RECALL		0xB8
#define DS18X20_READ_PS			0xB4

#define	DS1820_CMD_READY		0
#define	ERR_MAX					200
#define	CRC_COMPUTE
#define	PROJ_1WIRE_SLAVE_TEMPERATURE
//#define	ALT_MINUS_DISPLAY					;alternating display - sign and temperature (below zero)

.equ	T1TOP		= 156						;~1ms timer prescaler = 128 @ 20MHz
.equ	T1INIT		= (1<<CTC1|1<<PWM1A|1<<CS13);prescaler = 128, clear timer on compare match OCR3C

.equ	disp_size	= 3							;3 digits size
.equ	dp			= 0x80						;decimal point
.equ	pat_minus	= 0x40						;pattern for - sign



.cseg										;Code-Segment
.org	0
 		rjmp 	reset
.org	OC1Aaddr
		rjmp	ISR_COMP1A					;TIM1_COMPA


.org	INT_VECTORS_SIZE					;reserve space for other int vectors

.include	"w1-slave.asm"
.org		w1_slave_cseg_end
.include	"crc.asm"
.org		crc_cseg_end
.include	"w1.asm"
.org		w1_cseg_end




.dseg
datastart:
w1_1820_ok:	.byte	1
err_cnt:	.byte	1
timer_1ms:	.byte	1
timer_5ms:	.byte	1
timer_1s:	.byte	1
para_delay:	.byte	1
disp_state:	.byte	1
disp_buff:	.byte	6
disp_buff_end:
ds1820_scr:	.byte	9
ds1820_ee:	.byte	3
temp_sign:	.byte	1
temp_100s:	.byte	1
temp_10s:	.byte	1
temp_1s:	.byte	1
temp_frc10:	.byte	1
temp_frc100:.byte	1
dataend:


.cseg										;Code-Segment

;process other commands, X = pointer to buffer, inc and check self "cpi XL,LOW(w1_buffer_end)"
;byte 0 = romcmd, byte 1 = devcmd, numbyte = actual sequence > 1
w1_data_other:
		lds		tmp0,w1_devcmd				;load devcmd
		cpi		tmp0,DS18X20_CONVERT_T
		breq	w1_ds18x20_convert_t
		cpi		tmp0,DS18X20_READ
		breq	w1_ds18x20_read
		cpi		tmp0,DS18X20_WRITE
		breq	w1_ds18x20_write
		cpi		tmp0,DS18X20_EE_WRITE
		breq	w1_ds18x20_ee_write
		cpi		tmp0,DS18X20_EE_RECALL
		breq	w1_ds18x20_ee_recall
		cpi		tmp0,DS18X20_READ_PS
		breq	w1_ds18x20_read_ps

w1_ds18x20_read_ps:
w1_ds18x20_convert_t:
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		ser		tmp0						;send 0xff -> conversion ready
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit

w1_ds18x20_read:
		lds		tmp0,w1_numbyte				;first byte -> setup buffer
		cpi		tmp0,1
		brne	w1_ds18x20_read_run
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		push	YL
		push	YH
		ldi		YL,LOW(ds1820_scr)			;load startaddress scratchpad buffer
		ldi		YH,HIGH(ds1820_scr)
		ldi		tmp1,9						;copy 9 byte
w1_ds18x20_read_copy_loop:
		ld		tmp0,Y+						;copy ds1820_scr to w1_buffer
		st		X+,tmp0
		dec		tmp1
		brne	w1_ds18x20_read_copy_loop
		pop		YH
		pop		YL
		ldi		XL,LOW(w1_buffer)			;load startaddress buffer
		ldi		XH,HIGH(w1_buffer)
w1_ds18x20_read_run:
		lds		tmp0,w1_numbyte				;last byte -> stop
		cpi		tmp0,10
		brne	w1_ds18x20_read_load		;no send data
		ldi		w1in_stat,1<<W1IN_IDLE		;command complete -> idle state
		rjmp	w1_byte_ready_exit_ptr
w1_ds18x20_read_load:
		ld		tmp0,X+						;load data byte
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit

w1_ds18x20_write:
		lds		tmp0,w1_numbyte				;first byte -> devcmd -> setup pointer
		cpi		tmp0,1
		brne	w1_ds18x20_write_run
		ldi		XL,LOW(ds1820_ee)
		ldi		XH,HIGH(ds1820_ee)
		rjmp	w1_byte_ready_exit
w1_ds18x20_write_run:
		lds		tmp0,w1_shift				;load data
		st		X+,tmp0						;store byte in buffer
		lds		tmp0,w1_numbyte				;last byte -> stop
		cpi		tmp0,4
		brne	w1_ds18x20_write_exit
		ldi		w1in_stat,1<<W1IN_IDLE		;command complete -> idle state
		rjmp	w1_data_other_ok			;stop, signal
w1_ds18x20_write_exit:
		rjmp	w1_byte_ready_exit

w1_ds18x20_ee_recall:
w1_ds18x20_ee_write:
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		clr		tmp0						;ee write/recall in progress -> send 0s
		sts		w1_shift,tmp0
		lds		tmp0,w1_numbyte				;first byte -> setup w1_1820_ok
		cpi		tmp0,1
		breq	w1_data_other_ok
		ser		tmp0						;ee write/recall complete -> send 1s
		lds		tmp1,w1_1820_ok				;command ready?
		sbrc	tmp1,DS1820_CMD_READY
		clr		tmp0						;ee write/recall in progress -> send 0s
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit

w1_data_other_ok:
		lds		tmp0,w1_1820_ok				;load signal byte
		sbr		tmp0,1<<DS1820_CMD_READY	;set signal command ready
		sts		w1_1820_ok,tmp0
		rjmp	w1_byte_ready_exit_ptr


;interrupt every 1ms, update counters, output multiplex display
ISR_COMP1A:									;TIM1_COMP1A
		in		int_sr,SREG
		push	int_sr
		push	tmp0						;use tmp0 because int_tmp is manipulated
		sei									;allow 1wire-slave interrupts
		lds		tmp0,timer_1ms				;update ms timer
		inc		tmp0
		sts		timer_1ms,tmp0
		cpi		tmp0,5						;5ms ?
		brne	comp1a_no_5msec
		clr		tmp0						;yes -> reset timer
		sts		timer_1ms,tmp0
		lds		tmp0,timer_5ms				;update 5ms timer
		inc		tmp0
		sts		timer_5ms,tmp0
		lds		tmp0,disp_state
		inc		tmp0
		cpi		tmp0,3						;state wrap?
		brlo	comp1a_disp_state_no_wrap
		clr		tmp0						;reset to first byte
comp1a_disp_state_no_wrap:
		sts		disp_state,tmp0
		rcall	digit_actual_out			;output multiplex digit
		lds		tmp0,para_delay				;parasite power cycle?
		tst		tmp0
		breq	comp1a_no_parasite
		dec		tmp0						;dec parasite power timer
		sts		para_delay,tmp0
		brne	comp1a_no_parasite			;switch power off?
		rcall	w1_parasite_off
comp1a_no_parasite:
		lds		tmp0,timer_5ms				;check 10ms timer
		cpi		tmp0,200					;1s ?
		brne	comp1a_no_5msec
		clr		tmp0						;yes -> reset timer
		sts		timer_5ms,tmp0
		lds		tmp0,timer_1s				;update s timer
		inc		tmp0
		sts		timer_1s,tmp0
comp1a_no_5msec:
		pop		tmp0
		pop		int_sr
		out		SREG,int_sr
		reti

;output actual digit to multiplex display
digit_actual_out:
		push	XL
		push	XH
		push	tmp0
		push	tmp1
		push	tmp2
		ldi		XL,LOW(disp_buff)
		ldi		XH,HIGH(disp_buff)
		lds		tmp0,timer_1s
		sbrc	tmp0,0
		adiw	XL,3						;second display set
		lds		tmp0,disp_state				;load actual digit byte number
		clr		tmp1						;fur adc
		add		XL,tmp0						;compute address
		adc		XH,tmp1
		mov		tmp2,tmp0					;shift counter for digit bit shift
		ld		tmp0,X						;load data
		ldi		tmp1,1
digit_actual_digit_shift:
		tst		tmp2
		breq	digit_actual_digit_out
		lsl		tmp1
		dec		tmp2
		brne	digit_actual_digit_shift
digit_actual_digit_out:
		lds		tmp2,temp_sign				;set minus led
		or		tmp1,tmp2
		rcall	digit_out
digit_actual_exit:
		pop		tmp2
		pop		tmp1
		pop		tmp0
		pop		XH
		pop		XL
		ret

;output actual digit to shift registers
digit_out:
		ldi		tmp2,16						;output 16 bits
		cbi		PORTB,PB0					;output 0
digit_out_clr_loop:
		sbi		PORTB,PB1					;shift clock pulse
		cbi		PORTB,PB1
		dec		tmp2
		brne	digit_out_clr_loop
		ldi		tmp2,16						;output 16 bits
		cbi		PORTB,PB0					;output 0
		ori		tmp1,0x80					;set highest bit -> register clock
digit_out_loop:
		cbi		PORTB,PB1					;shift clock pulse end
		sbrs	tmp1,7
		cbi		PORTB,PB0					;output 0
		sbrc	tmp1,7
		sbi		PORTB,PB0					;output 1
		rol		tmp0						;second output digit
		rol		tmp1						;first output digit driver bit
		sbi		PORTB,PB1					;shift clock pulse
		dec		tmp2
		brne	digit_out_loop
		sbi		PORTB,PB0					;final register clock pulse
		nop
		nop
		nop
		nop
		nop
		cbi		PORTB,PB0
		cbi		PORTB,PB1
		ret

;convert byte in tmp0 to 7 segment pattern, return tmp0
digit_conv:
		push	ZL
		push	ZH
		ldi		ZL,LOW(2*x7seg_pattern)
		ldi		ZH,HIGH(2*x7seg_pattern)
		andi	tmp0,0x1f					;keep input in range
		add		ZL,tmp0
		ldi		tmp0,0						;for adc
		adc		ZH,tmp0
		lpm		tmp0,Z
		pop		ZH
		pop		ZL
		ret

;copy first display sequence to second one
digit_out_copy:
		push	tmp0
		lds		tmp0,disp_buff
		sts		disp_buff+3,tmp0
		lds		tmp0,disp_buff+1
		sts		disp_buff+4,tmp0
		lds		tmp0,disp_buff+2
		sts		disp_buff+5,tmp0
		pop		tmp0
		ret

;output minus sign on second sequence
digit_out_minus:
		push	tmp0
		ldi		tmp0,pat_minus
		sts		disp_buff+3,tmp0
		clr		tmp0
		sts		disp_buff+4,tmp0
		sts		disp_buff+5,tmp0
		pop		tmp0
		ret

;output hex byte to display buffer at position X
hex_out:
		push	tmp0						;save byte
		swap	tmp0						;first high nibble
		andi	tmp0,0x0f
		rcall	digit_conv
		st		X+,tmp0						;store high nibble
		pop		tmp0
		push	tmp0
		andi	tmp0,0x0f					;now low nibble
		rcall	digit_conv
		st		X+,tmp0						;store low nibble
		pop		tmp0
		ret

;output error to display buffer, err number in tmp0
err_out:
		push	XL
		push	XH
		push	tmp0
		lds		tmp0,err_cnt				;update err_cnt
		inc		tmp0
		sts		err_cnt,tmp0
		ldi		XL,LOW(disp_buff)
		ldi		XH,HIGH(disp_buff)
		ldi		tmp0,0x0e					;output "Err"
		rcall	digit_conv
		st		X+,tmp0						;store to buffer
		ldi		tmp0,19						;pattern "r"
		rcall	digit_conv
		st		X+,tmp0						;store to buffer
		st		X+,tmp0
		clr		tmp0						;sequence 2 digit 1 -> blank
		st		X+,tmp0
		pop		tmp0						;restore err number
		push	tmp0
		rcall	hex_out						;output to buffer
		pop		tmp0
		pop		XH
		pop		XL
		ret

;execute convert command, pointer to rom in Y
ds18x20_convert:
		push	tmp0
		push	tmp1
		ldi		tmp0,DS18X20_CONVERT_T		;command convert temperature
		rcall	w1_command
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)	;error bit mask
		breq	ds18x20_convert_cmd_ok
		rjmp	ds18x20_convert_exit
ds18x20_convert_cmd_ok:
		ld		tmp0,Y						;check id[0] no DS18X20 -> no parasite
		cpi		tmp0,CHIP_DS18B20
		breq	ds18x20_convert_para_ok
		cpi		tmp0,CHIP_DS18S20
		breq	ds18x20_convert_para_ok
		rjmp	ds18x20_convert_exit
ds18x20_convert_para_ok:
		rcall	w1_parasite_on
		ldi		tmp0,150					;set parasite power delay to 150 x 5ms
		sts		para_delay,tmp0
ds18x20_convert_exit:
		pop		tmp1
		pop		tmp0
		ret

;read scratchpad (9 bytes), check crc rom in Y, scratchpad buffer in X
ds18x20_read:
		push	XL
		push	XH
		push	YL
		push	YH
		push	tmp0
		push	tmp1
		ldi		tmp0,DS18X20_READ			;command read scratchpad
		rcall	w1_command
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)	;error bit mask
		breq	ds18x20_read_cmd_ok
		rjmp	ds18x20_read_exit
ds18x20_read_cmd_ok:
		push	XL							;save data buffer address
		push	XH
		ldi		tmp1,9
ds18x20_read_loop:
		ser		tmp0
		rcall	w1_byte_io
		st		X+,tmp0
		dec		tmp1
		brne	ds18x20_read_loop
		pop		YH							;restore data buffer address
		pop		YL
		andi	w1_stat,~(1<<W1_CRC_ERR)	;reset error flag
		ldi		tmp1,8						;8 byte crc
		rcall	w1_crc
		lds		tmp0,crc8_data				;load computed crc
		ld		tmp1,Y+						;load crc of scratchpad
		cpse	tmp0,tmp1					;correct?
		ori		w1_stat,1<<W1_CRC_ERR		;no, set error flag
ds18x20_read_exit:
		pop		tmp1
		pop		tmp0
		pop		YH
		pop		YL
		pop		XH
		pop		XL
		ret

;write data to eeprom scratchpad
ds18x20_write:
		push	XL
		push	XH
		push	tmp0
		push	tmp1
		ldi		XL,LOW(ds1820_ee)
		ldi		XH,HIGH(ds1820_ee)
		ldi		tmp0,DS18X20_WRITE			;command write scratchpad
		rcall	w1_command
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)	;error bit mask
		breq	ds18x20_write_cmd_ok
		rjmp	ds18x20_write_exit
ds18x20_write_cmd_ok:
		ldi		tmp1,3						;write 3 byte
ds18x20_write_loop:
		ld		tmp0,X+						;load data
		rcall	w1_byte_io
		dec		tmp1
		brne	ds18x20_write_loop
ds18x20_write_exit:
		pop		tmp1
		pop		tmp0
		pop		XH
		pop		XL
		ret

;send command copy to eeprom
ds18x20_ee_write:
		push	tmp0
		push	tmp1
		ldi		tmp0,DS18X20_EE_WRITE		;command copy eeprom
		rcall	w1_command
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)	;error bit mask
		breq	ds18x20_ee_write_cmd_ok
		rjmp	ds18x20_ee_write_exit
ds18x20_ee_write_cmd_ok:
		rcall	w1_parasite_on				;parasite power for 10ms
		ldi		tmp1,HIGH(10000)			;10ms delay
		ldi		tmp0,LOW(10000)
		rcall	delay_us
		rcall	w1_parasite_off
ds18x20_ee_write_exit:
		pop		tmp1
		pop		tmp0
		ret

;send command copy to eeprom
ds18x20_ee_recall:
		push	tmp0
		push	tmp1
		ldi		tmp0,DS18X20_EE_RECALL		;command recall eeprom
		rcall	w1_command
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)	;error bit mask
		breq	ds18x20_ee_recall_loop
		rjmp	ds18x20_ee_recall_exit
ds18x20_ee_recall_loop:
		ser		tmp0
		rcall	w1_byte_io
		tst		tmp0						;recall, in progress (read 0s)?
		breq	ds18x20_ee_recall_loop
ds18x20_ee_recall_exit:
		pop		tmp1
		pop		tmp0
		ret

;compute temperature, store digits at separate variables
ds18x20_compute:
		push	YL
		push	YH
		push	tmp0
		push	tmp1
		push	tmp2
		andi	w1_stat,~(1<<W1_CRC_ERR)	;reset error flag
		ldi		tmp1,8						;8 byte crc
		rcall	w1_crc
		lds		tmp0,crc8_data				;load computed crc
		ld		tmp1,Y+						;load crc of scratchpad
		cpse	tmp0,tmp1					;correct?
		ori		w1_stat,1<<W1_CRC_ERR		;no, set error flag
		cpse	tmp0,tmp1					;correct?
		rjmp	ds18x20_compute_exit
		lds		tmp0,ds1820_scr				;load LSB
		lds		tmp1,ds1820_scr+1			;load MSB
		lds		tmp2,ds1820_scr+4			;load config register -> check sensor type
		sbrs	tmp2,7						;bit 7 set -> DS18S20
		rjmp	ds18x20_compute_ds18b20
		ldi		tmp2,3
ds18x20_compute_shift_18s20:
		clc
		rol		tmp0
		rol		tmp1						;shift to same values like 18b20
		dec		tmp2
		brne	ds18x20_compute_shift_18s20
		subi	tmp0,4						;sub 2^-2 -> 0.25
		sbci	tmp1,0
		ldi		tmp3,16
		lds		tmp2,ds1820_scr+6			;load count remain
		sub		tmp3,tmp2					;16 - count remain
		clr		tmp2						;for adc
		add		tmp0,tmp3					;add 0.0625 * (16 - count remain)
		adc		tmp1,tmp2
ds18x20_compute_ds18b20:
		clr		tmp2
		tst		tmp1						;temperature negative?
		brpl	ds18x20_compute_shift_start
		neg		tmp0						;yes -> 2s complement LSB
		com		tmp1						;1s complement MSB
		ldi		tmp2,pat_minus				;load minus
ds18x20_compute_shift_start:
		sts		temp_sign,tmp2				;store sign
		push	tmp0						;save fractional part
		ldi		tmp2,4
ds18x20_compute_shift_frac:
		asr		tmp1						;shift fractional part out
		ror		tmp0
		dec		tmp2
		brne	ds18x20_compute_shift_frac
		subi	tmp0,100					;temperature > 100?
		sbci	tmp1,0
		brpl	ds18x20_compute_100_plus
		ldi		tmp2,100
		add		tmp0,tmp2					;undo last sub
		clr		tmp2
		adc		tmp1,tmp2
		ldi		tmp2,0						;store 100s
		rjmp	ds18x20_compute_10
ds18x20_compute_100_plus:
		ldi		tmp2,1						;store 100s
ds18x20_compute_10:
		sts		temp_100s,tmp2
		ldi		tmp2,-1
ds18x20_compute_10_plus:
		inc		tmp2
		subi	tmp0,10						;temperature > 10?
		sbci	tmp1,0
		brpl	ds18x20_compute_10_plus
		subi	tmp0,-10					;undo last sub
		sts		temp_10s,tmp2				;store 10s
		sts		temp_1s,tmp0				;store 1s
		pop		tmp0						;load LSB, fraction
		push	ZL
		push	ZH
		ldi		ZL,LOW(2*ds18x20_fraction_10)
		ldi		ZH,HIGH(2*ds18x20_fraction_10)
		andi	tmp0,0x0f					;keep input in range
		push	tmp0						;for frac100
		add		ZL,tmp0
		ldi		tmp0,0						;for adc
		adc		ZH,tmp0
		lpm		tmp0,Z
		sts		temp_frc10,tmp0
		pop		tmp0
		ldi		ZL,LOW(2*ds18x20_fraction_100)
		ldi		ZH,HIGH(2*ds18x20_fraction_100)
		add		ZL,tmp0
		ldi		tmp0,0						;for adc
		adc		ZH,tmp0
		lpm		tmp0,Z
		sts		temp_frc100,tmp0
		pop		ZH
		pop		ZL
ds18x20_compute_exit:
		pop		tmp2
		pop		tmp1
		pop		tmp0
		pop		YH
		pop		YL
		ret

;output temperature digits to display buffer
ds18x20_out:
		push	XL
		push	XH
		push	tmp0
		push	tmp1
		ldi		XL,LOW(disp_buff)
		ldi		XH,HIGH(disp_buff)
		lds		tmp0,temp_100s				;load and output 100s
		tst		tmp0
		breq	ds18x20_out_no_100
		rcall	digit_conv
		st		X+,tmp0
		rjmp	ds18x20_out_10				;100s present -> always output 10s
ds18x20_out_no_100:
		lds		tmp0,temp_10s				;load and output 10s
		tst		tmp0
		breq	ds18x20_out_no_10
ds18x20_out_10:
		lds		tmp0,temp_10s				;load and output 10s
		rcall	digit_conv
		st		X+,tmp0
ds18x20_out_no_10:
		lds		tmp0,temp_1s				;load and output ones
		rcall	digit_conv
		st		X+,tmp0						;store ones without decimal point
		cpi		XL,LOW(disp_buff+3)
		brsh	ds18x20_out_sign
		ori		tmp0,dp						;ones -> decimal point
		sbiw	XL,1						;one address back
		st		X+,tmp0						;store ones with decimal point
		cpi		XL,LOW(disp_buff+2)			;how much space available for display?
		brsh	ds18x20_out_frc10
		lds		tmp0,temp_frc100			;load and output fraction 100s
		rcall	hex_out
		rjmp	ds18x20_out_sign
ds18x20_out_frc10:
		lds		tmp0,temp_frc10				;load and output fraction 10s
		rcall	digit_conv
		st		X+,tmp0						;store ones without decimal point
ds18x20_out_sign:
#ifdef ALT_MINUS_DISPLAY
		lds		tmp0,temp_sign				;output minus?
		tst		tmp0
		breq	ds18x20_out_copy
		rcall	digit_out_minus
		rjmp	ds18x20_out_exit
#endif
ds18x20_out_copy:
		rcall	digit_out_copy
ds18x20_out_exit:
		pop		tmp1
		pop		tmp0
		pop		XH
		pop		XL
		ret

;delay 2s for smooth display
display_delay:
		push	tmp0
		push	tmp1
		ldi		tmp0,150					;150 x 10ms delay
display_delay_loop:
		push	tmp0
		ldi		tmp1,HIGH(10000)			;10ms delay
		ldi		tmp0,LOW(10000)
		rcall	delay_us
		pop		tmp0
		subi	tmp0,1						;200 x 10ms = 2s reached?
		brne	display_delay_loop
		pop		tmp1
		pop		tmp0
		ret

reset:
		ldi		tmp0,1<<CLKPCE				;Clock prescaler change enable
		out		CLKPR,tmp0
		clr		tmp0						;no prescaler
		out		CLKPR,tmp0

		ldi		XL,LOW(datastart)
		ldi		XH,HIGH(datastart)
reset_data_loop:
		st		X+,tmp0						;init vars -> clear all ram locations
		cpi		XL,LOW(dataend)
		brne	reset_data_loop
		ldi		tmp0,pat_minus				;load minus
		sts		temp_sign,tmp0				;store sign for correct multiplex out

		ldi		tmp0,T1TOP					;max value -> clear timer1
		out		OCR1C,tmp0
		out		OCR1A,tmp0					;interrupt on max value
		ldi		tmp0,T1INIT					;timer1 clock CK/1024, clear on compare match OCR1C
		out		TCCR1,tmp0					;start timer1
		in		tmp0,TIMSK					;enable timer1 output compare1a interrupt
		ori		tmp0,1<<OCIE1A
		out		TIMSK,tmp0
		sbi		DDRB,PB0					;shift register data
		sbi		DDRB,PB1					;shift register clock/strobe


err_loop:
		rcall	w1_slave_init

		ldi		tmp0,20						;pattern #20 -> lamp test
		rcall	digit_conv
		ldi		XL,LOW(disp_buff)
		ldi		XH,HIGH(disp_buff)
segbuff_fill_loop_lt:
		st		X+,tmp0
		cpi		XL,LOW(disp_buff_end)
		brne	segbuff_fill_loop_lt
		rcall	display_delay
		clr		tmp0
		sts		temp_sign,tmp0				;lamp test end -> switch off minus sign

		ldi		YL,LOW(w1_id)				;output id byte by byte
		ldi		YH,HIGH(w1_id)
		clr		tmp2						;loopcounter
display_id_loop:
		ldi		XL,LOW(disp_buff)
		ldi		XH,HIGH(disp_buff)
		mov		tmp0,tmp2					;convert sequence to first pattern
		ori		tmp0,0x18					;special count pattern
		rcall	digit_conv
		st		X+,tmp0						;store at display buffer
		ld		tmp0,Y+						;load id byte
		rcall	hex_out						;output to buffer
		rcall	digit_out_copy				;copy to second sequence
		rcall	display_delay
		inc		tmp2
		cpi		tmp2,8						;output 8 byte id
		brne	display_id_loop

		clr		tmp0						;reset signal command ready
		sts		w1_1820_ok,tmp0
		sts		err_cnt,tmp0				;reset err_cnt

main:
		ldi		YL,LOW(w1_id)				;address rom id
		ldi		YH,HIGH(w1_id)
		rcall	ds18x20_convert
main_parasite_wait:
		wdr									;watchdogreset
		lds		tmp0,w1_1820_ok				;command ready?
		sbrs	tmp0,DS1820_CMD_READY
		rjmp	main_parasite_check
		rcall	cmd_exec
		rjmp	main
main_parasite_check:
		lds		tmp0,para_delay				;wait for parasite power?
		tst		tmp0
		brne	main_parasite_wait
		ldi		XL,LOW(ds1820_scr)			;address data buffer
		ldi		XH,HIGH(ds1820_scr)
		ldi		YL,LOW(w1_id)				;address rom id
		ldi		YH,HIGH(w1_id)
		rcall	ds18x20_read
		ldi		YL,LOW(ds1820_scr)			;address data buffer
		ldi		YH,HIGH(ds1820_scr)
		rcall	ds18x20_compute				;compute temperature from scratchpad
		mov		tmp0,w1_stat				;check for errors
		andi	tmp0,~(1<<W1_SB1|1<<W1_SB0)	;error bit mask
		breq	main_output
		rcall	err_out
		ldi		tmp1,HIGH(50000)			;50ms delay
		ldi		tmp0,LOW(50000)
		rcall	delay_us
		lds		tmp0,err_cnt				;max errors?
		cpi		tmp0,ERR_MAX
		brne	main_noerr
		rjmp	err_loop
main_noerr:
		rjmp	main
main_output:
		rcall	ds18x20_out					;put to output buffer
		rjmp	main



;execute command
cmd_exec:
		rcall	w1_parasite_off				;interrupt current cycle
		lds		tmp0,w1_devcmd				;load devcmd
		cpi		tmp0,DS18X20_WRITE
		breq	cmd_exec_write
		cpi		tmp0,DS18X20_EE_WRITE
		breq	cmd_exec_ee_write
		cpi		tmp0,DS18X20_EE_RECALL
		breq	cmd_exec_ee_recall
		rjmp	cmd_exec_exit				;unknown command
cmd_exec_write:
		ldi		YL,LOW(w1_id)				;address rom id
		ldi		YH,HIGH(w1_id)
		rcall	ds18x20_write
		rjmp	cmd_exec_exit
cmd_exec_ee_write:
		rcall	ds18x20_ee_write
		rjmp	cmd_exec_exit
cmd_exec_ee_recall:
		rcall	ds18x20_ee_recall
cmd_exec_exit:
		lds		tmp0,w1_1820_ok				;reset command ready
		cbr		tmp0,1<<DS1820_CMD_READY
		sts		w1_1820_ok,tmp0
		ret


x7seg_pattern:
		.db		0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07
		.db		0x7f, 0x6f, 0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71
		.db		0x01, 0x40, 0x08, 0x50, 0xff, 0xff, 0x00, 0x00
		.db		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80

ds18x20_fraction_10:
		.db		0x00, 0x01, 0x01, 0x02, 0x03, 0x03, 0x04, 0x04
		.db		0x05, 0x06, 0x06, 0x07, 0x08, 0x08, 0x09, 0x09

ds18x20_fraction_100:
		.db		0x00, 0x06, 0x13, 0x19, 0x25, 0x31, 0x38, 0x44
		.db		0x50, 0x56, 0x63, 0x69, 0x75, 0x81, 0x88, 0x94
