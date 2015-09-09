/**
 * 1wire slave implementation for connecting a serial LCD
 * 
 * This program receives 1wire commands and sends it to a serial connected
 * LCD display with standard HD44780 controller. Refer to the "lcd-serial-protocol"
 * and "lcd-1wire-protocol" and the schematics documents an how to use this software.
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

;device commands w1_devcmd
.equ	DC_LCD_CLEAR		= 0x1f
.equ	DC_LCD_READ			= 0xab
.equ	DC_LCD_WRITE		= 0x56
.equ	DC_LCD_RAW_READ		= 0xf6
.equ	DC_LCD_RAW_WRITE	= 0x16
.equ	DC_LCD_PWM			= 0x6a

.equ	STX					= 0x02
.equ	ETX					= 0x03


.cseg										;Code-Segment
.org	0
 		rjmp 	reset

.org	INT_VECTORS_SIZE					;reserve space for other int vectors

.include	"w1-slave.asm"
.org		w1_slave_cseg_end
.include	"crc.asm"
.org		crc_cseg_end
.include	"w1.asm"
.org		w1_cseg_end
.include	"uart-soft-fd.asm"
.org		uart_soft_fd_cseg_end

.equ	LCD_CMD_READY		= 0
.equ	LCD_DO_READ			= 1

.dseg
w1_lcd_ok:	.byte	1
w1_lcd_num:	.byte	1


.cseg										;Code-Segment

;process other commands, X = pointer to buffer, inc and check self "cpi XL,LOW(w1_buffer_end)"
;byte 0 = romcmd, byte 1 = devcmd, numbyte = actual sequence > 1
w1_data_other:
		lds		tmp0,w1_numbyte				;sequence byte number, byte 0 = romcmd, byte 1 = devcmd
		cpi		tmp0,2						;byte 2 -> numbytes for lcd
		brne	w1_data_other_run
		clr		tmp0						;clear signals
		sts		w1_lcd_ok,tmp0
		lds		tmp0,w1_shift				;load number of data bytes in frame
		rcall	crc16
		cpi		tmp0,W1_BUFFER_SIZE+1		;check maximum numbytes
		brlo	w1_data_other_num_ok
		ldi		tmp0,W1_BUFFER_SIZE+1		;load max
w1_data_other_num_ok:
		subi	tmp0,-5						;add 5 bytes for num, crc16(2) and execute byte
		sts		w1_lcd_num,tmp0				;save end number and immediate check to send crc
		cpi		tmp0,5						;0 byte data packet?
		breq	w1_data_lcd_crc				;yes -> immediate send crc
		rjmp	w1_byte_ready_exit
w1_data_other_run:
		lds		tmp1,w1_lcd_num
		sub		tmp1,tmp0					;how many bytes are left?
		breq	w1_data_lcd_ready
		cpi		tmp1,3						;3 bytes -> last byte -> store and send crc low
		breq	w1_data_lcd_last
		cpi		tmp1,2						;2 bytes -> send crc high
		breq	w1_data_lcd_crc_hi
		cpi		tmp1,1						;1 byte -> send 0xff
		breq	w1_data_lcd_crc_hi
		lds		tmp0,w1_lcd_ok				;send answer?
		sbrc	tmp0,LCD_DO_READ
		rjmp	w1_data_lcd_read
		lds		tmp0,w1_shift				;else load data byte
		st		X+,tmp0						;and store in buffer
		rcall	crc16						;update crc
		rjmp	w1_byte_ready_exit
w1_data_lcd_read:
		cpi		tmp1,4						;4 byte -> send crc low
		brne	w1_data_lcd_read_data
		lds		tmp1,w1_lcd_num				;set signal to send crc high on next cycle
		dec		tmp1
		sts		w1_lcd_num,tmp1
		rjmp	w1_data_lcd_crc				;send crc
w1_data_lcd_read_data:
		ld		tmp0,X+						;load data
		sts		w1_shift,tmp0				;setup data to send
		rcall	crc16						;update crc
		rjmp	w1_byte_ready_exit
w1_data_lcd_last:
		lds		tmp0,w1_shift				;load last data byte
		st		X,tmp0						;and store in buffer
		rcall	crc16						;update crc
w1_data_lcd_crc:
		sbr		w1in_stat,1<<W1IN_WRITE		;signal output
		lds		tmp0,crc16_data_l			;send low byte crc16
		sts		w1_shift,tmp0
		rjmp	w1_byte_ready_exit
w1_data_lcd_crc_hi:
		lds		tmp0,crc16_data_h			;send high byte crc16
		sts		w1_shift,tmp0
		ser		tmp0						;send 0xff for consecutive reads
		sts		crc16_data_h,tmp0
		lds		tmp0,w1_numbyte				;sequence byte number
		lds		tmp1,w1_lcd_num
		cpse	tmp0,tmp1					;command complete inclusive additional 0xff?
		rjmp	w1_byte_ready_exit
w1_data_lcd_ready:
		lds		tmp0,w1_lcd_ok				;load signal byte
		sbr		tmp0,1<<LCD_CMD_READY		;set signal command ready
		sts		w1_lcd_ok,tmp0
		lds		tmp0,w1_devcmd				;load active device command
		cpi		tmp0,DC_LCD_READ			;command read -> send answer data
		breq	w1_data_lcd_ready_read
		cpi		tmp0,DC_LCD_RAW_READ		;command raw read -> send answer data
		breq	w1_data_lcd_ready_read
		rjmp	w1_data_lcd_ready_exit
w1_data_lcd_ready_read:
		lds		tmp0,w1_lcd_ok				;load signal byte
		sbrs	tmp0,LCD_DO_READ			;second end of read sequence
		rjmp	w1_byte_ready_exit_ptr
		clr		tmp0						;clear signals
		sts		w1_lcd_ok,tmp0
w1_data_lcd_ready_exit:
		ldi		w1in_stat,1<<W1IN_IDLE		;command complete -> idle state
		rjmp	w1_byte_ready_exit_ptr





reset:
		ldi		tmp0,1<<CLKPCE				;Clock prescaler change enable
		out		CLKPR,tmp0
		clr		tmp0						;no prescaler
		out		CLKPR,tmp0
	
		rcall	w1_slave_init
		ldi		tmp0,0x72					;chip id
		rcall	w1_id_set_chip				;set chip id, compute and store crc
		rcall	w1_id_ok					;output id on serial output

		clr		tmp0						;reset signal command ready
		sts		w1_lcd_ok,tmp0
main:
		lds		tmp0,w1_lcd_ok				;reset command ready
		cbr		tmp0,1<<LCD_CMD_READY
		sts		w1_lcd_ok,tmp0
cmd_loop:
		wdr									;watchdogreset
		lds		tmp0,w1_lcd_ok				;command ready?
		sbrs	tmp0,LCD_CMD_READY
		rjmp	cmd_loop
		cbr		tmp0,1<<LCD_CMD_READY		;reset command ready
		sts		w1_lcd_ok,tmp0
		clr		tmp0						;stop timer
		out		TCCR0B,tmp0
		in		tmp0,GIMSK					;disable INT0 -> dont serve 1wire until command complete
		andi	tmp0,~(1<<INT0)
		out		GIMSK,tmp0
		lds		tmp0,w1_devcmd				;last complete command
		cpi		tmp0,DC_LCD_CLEAR
		breq	lcd_init_tmp
		cpi		tmp0,DC_LCD_WRITE
		breq	lcd_write_tmp
		cpi		tmp0,DC_LCD_RAW_READ
		breq	lcd_raw_read_tmp
		cpi		tmp0,DC_LCD_RAW_WRITE
		breq	lcd_raw_write_tmp
		cpi		tmp0,DC_LCD_READ
		breq	lcd_read_tmp
		cpi		tmp0,DC_LCD_PWM
		breq	lcd_pwm_tmp
		rjmp	main						;wait forever

lcd_init_tmp:
		rcall	lcd_init
		rcall	lcd_ena_int
		rcall	lcd_receive					;get command answer
		rjmp	main						;wait forever
lcd_write_tmp:
		rcall	lcd_write
		rcall	lcd_ena_int
		rcall	lcd_receive					;get command answer
		rjmp	main						;wait forever
lcd_raw_write_tmp:
		rcall	lcd_raw_write
		rcall	lcd_ena_int
		rcall	lcd_receive					;get command answer
		rjmp	main						;wait forever
lcd_pwm_tmp:
		rcall	lcd_pwm
		rcall	lcd_ena_int
		rcall	lcd_receive					;get command answer
		rjmp	main						;wait forever
lcd_read_tmp:
		rcall	lcd_read
		rjmp	lcd_read_all
lcd_raw_read_tmp:
		rcall	lcd_raw_read
lcd_read_all:
		rcall	lcd_ena_int
		rcall	lcd_receive					;get command answer
		clr		tmp1						;init crc
		sts		crc16_data_h,tmp1
		sts		crc16_data_l,tmp1
		lds		tmp0,w1_lcd_ok				;set signal read result ready
		sbr		tmp0,1<<LCD_DO_READ
		sts		w1_lcd_ok,tmp0
		cli									;inhibit interrupts to prevent pointer corruption
		lds		tmp1,w1_numbyte				;actual sequence number
		lds		tmp0,w1_lcd_num				;bytes to send as answer
		add		tmp0,tmp1					;send until this sequence number
		subi	tmp0,-3						;3 additional bytes for crc
		sts		w1_lcd_num,tmp0
		ldi		XL,LOW(w1_buffer)			;init data pointer to data store
		ldi		XH,HIGH(w1_buffer)
		ld		tmp0,X+
		sts		w1_shift,tmp0
		sts		w1_ptr,XL					;save data pointer
		sts		w1_ptr+1,XH
		sei
		rcall	crc16						;update crc for first byte
		rjmp	main						;wait forever


lcd_ena_int:
		ldi		tmp0,1<<INTF0				;clear pending interrupt
		out		GIFR,tmp0
		in		tmp0,GIMSK					;enable INT0
		ori		tmp0,1<<INT0
		out		GIMSK,tmp0
		ret

lcd_init:
		rcall	lcd_send_start_seq
		clr		tmp0						;0 data bytes
		rcall	crc16						;update crc
		clr		tmp0						;0 data bytes
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		rcall	lcd_send_end_seq
		ret

lcd_write:
		rcall	lcd_send_start_seq
		lds		tmp0,w1_lcd_num				;load number data bytes
		subi	tmp0,5						;restore original form write scratch
		sts		w1_lcd_num,tmp0				;save number
		push	tmp0						;save num data bytes
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ldi		XL,LOW(w1_buffer)			;init data pointer to data store
		ldi		XH,HIGH(w1_buffer)
		pop		tmp0						;restore num data bytes
lcd_write_loop:
		push	tmp0						;save loop counter
		ld		tmp0,X+						;load and send data
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		pop		tmp0						;restore loop counter
		dec		tmp0
		brne	lcd_write_loop				;write additional data bytes
		rcall	lcd_send_end_seq
		ret

lcd_read:
		rcall	lcd_send_start_seq
		ldi		tmp0,2						;2 data bytes
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ldi		XL,LOW(w1_buffer)			;init data pointer to data store
		ldi		XH,HIGH(w1_buffer)
		ld		tmp0,X+						;load and send address
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ld		tmp0,X+						;load and send num read
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		rcall	lcd_send_end_seq
		ret

lcd_raw_read:
		rcall	lcd_send_start_seq
		ldi		tmp0,0						;0 data bytes
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		rcall	lcd_send_end_seq
		ret

lcd_pwm:
lcd_raw_write:
		rcall	lcd_send_start_seq
		ldi		tmp0,1						;1 data bytes
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ld		tmp0,X						;load data byte
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		rcall	lcd_send_end_seq
		ret


lcd_send_start_seq:
		ldi		tmp0,STX					;send STX
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		clr		tmp0						;init crc16
		sts		crc16_data_l,tmp0
		sts		crc16_data_h,tmp0
		lds		tmp0,w1_devcmd				;last complete command
		rcall	crc16						;update crc
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ret

lcd_send_end_seq:
		lds		tmp0,crc16_data_l			;send low byte crc16
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		lds		tmp0,crc16_data_h			;send high byte crc16
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ldi		tmp0,ETX					;send ETX
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ret

lcd_receive_exit_tmp:
		ret

lcd_receive:
lcd_receive_state_stx:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit_tmp
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_stx		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		ldi		tmp0,STX					;STX received?
		cpse	rx_data,tmp0
		rjmp	lcd_receive_state_stx		;no -> wait
lcd_receive_state_cmd:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit_tmp
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_cmd		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		clr		tmp1						;init crc
		sts		crc16_data_h,tmp1
		sts		crc16_data_l,tmp1
		lds		tmp0,w1_devcmd
		cpse	rx_data,tmp0				;same command answer?
		rjmp	lcd_receive_exit			;no -> end
		rcall	crc16						;cmd -> crc
lcd_receive_state_num:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_num		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		mov		tmp0,rx_data				;read num bytes
		rcall	crc16						;num -> crc
		cpi		tmp0,W1_BUFFER_SIZE+1		;max W1_BUFFER_SIZE (32) byte data?
		brsh	lcd_receive_exit			;too big data -> end
lcd_receive_state_data:
		ldi		XL,LOW(w1_buffer)			;init data pointer to data store
		ldi		XH,HIGH(w1_buffer)
		mov		tmp1,tmp0					;load to loopcounter
		sts		w1_lcd_num,tmp1				;and save for crc loopcount
		tst		tmp1						;packet with 0 byte data?
		breq	lcd_receive_state_crc
lcd_receive_state_data_loop:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_data_loop	;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		mov		tmp0,rx_data				;get received byte
		rcall	crc16						;data -> crc
		st		X+,tmp0						;store in data buffer
		dec		tmp1						;more data?
		brne	lcd_receive_state_data_loop
lcd_receive_state_crc:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_crc		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		mov		tmp0,rx_data				;get received byte
		lds		tmp1,crc16_data_l			;check crc
		cpse	tmp0,tmp1
		rjmp	lcd_receive_exit			;wrong crc -> end
lcd_receive_state_crc_h:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_crc_h		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		mov		tmp0,rx_data				;get received byte
		lds		tmp1,crc16_data_h			;check crc
		cpse	tmp0,tmp1
		rjmp	lcd_receive_exit			;wrong crc -> end
lcd_receive_state_etx:
		tst		w1in_stat					;w1 again busy?
		breq	lcd_receive_exit
		sbrs	uart_stat,rx_ready			;byte received?
		rjmp	lcd_receive_state_etx		;no -> wait
		cbr		uart_stat,1<<rx_ready		;clear signal
		ldi		tmp0,ETX					;ETX received?
		cpse	rx_data,tmp0
		rjmp	lcd_receive_exit			;no -> end

lcd_receive_exit:
		ret

