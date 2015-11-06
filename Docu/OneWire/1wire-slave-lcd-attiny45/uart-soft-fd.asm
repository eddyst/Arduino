/**
 * interrupt driven software-uart, full-duplex
 * 
 * This file implements a full duplex uart that uses a pin-change-interrupt and a
 * TIMERs OCR1A and OCR1B interrupt. Data is double buffered in a data- and
 * shiftregister.
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
.def	rx_data		= R0
.def	rx_shift	= R1
.def	rx_bitcnt	= R2
.def	tx_data		= R3
.def	tx_shift	= R4
.def	tx_bitcnt	= R5
.def	uart_stat	= R16
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

;port and pin definitions
.equ	RxD			= 0					;RxD = PXx
.equ	TxD			= 1					;TxD = PXx
.equ	FD_PCINT	= PCINT0			;pin change interrupt for RxD
.equ	FD_PORT		= PORTB
.equ	FD_DDR		= DDRB
.equ	FD_PIN		= PINB

;constants
.equ	SB			= 1					;stop bits 1,2...
.equ	FD_PRES		= 1					;presacler
.equ	T1TOP		= 174				;RS232 1-bit-timer 115200 @ 20MHz 174
.equ	T1INIT		= (1<<CTC1)|(LOG2(FD_PRES)+1)

;status bits
.equ	rx_start	= 0
.equ	rx_ready	= 1
.equ	tx_busy		= 2



.cseg									;Code-Segment

;start bit pin change interrupt
ISR_PCINT0:
		sbic	FD_PIN,RxD				;startbit -> low
		reti							;else exit
		in		int_sr,SREG
		in		int_tmp,TCNT1			;load actual timer for OCR1A
		subi	int_tmp,T1TOP/2+20/FD_PRES	;first cycle -> 1/2 bits
		brpl	isr_pcint_ocr			;subi overflow?
		subi	int_tmp,(256-T1TOP)		;yes -> add 1 bit time
isr_pcint_ocr:
		out		OCR1A,int_tmp
		sbr		uart_stat,1<<rx_start	;signal start bit receive
		ldi		int_tmp,1<<OCF1A		;clear output compare1a flag
		out		TIFR,int_tmp
		in		int_tmp,TIMSK			;enable timer1 output compare1a interrupt
		ori		int_tmp,1<<OCIE1A
		out		TIMSK,int_tmp
		in		int_tmp,GIMSK			;disable PCI
		andi	int_tmp,~(1<<PCIE)
		out		GIMSK,int_tmp
		ldi		int_tmp,0				;init shift register and bit counter
		mov		rx_shift,int_tmp
		ldi		int_tmp,8+SB
		mov		rx_bitcnt,int_tmp
		out		SREG,int_sr
		reti

;receive bit interrupt
ISR_COMP1A:
		in		int_sr,SREG
		sbrs	uart_stat,rx_start		;first cycle (1/2 bit)? 
		rjmp	isr_cm1a_sample
		in		int_tmp,TCNT1			;load counter for complete bit -> OCR1A
		out		OCR1A,int_tmp
		cbr		uart_stat,1<<rx_start	;signal rx running
		out		SREG,int_sr
		reti
isr_cm1a_sample:
		clc								;first clear carry
		sbic 	FD_PIN,RxD				;if RxD pin low
		sec								;set carry
		ror 	rx_shift				;shift bit into Rxbyte
		dec 	rx_bitcnt				;last bit?
		breq	isr_cm1a_stop			;yes, update status
		ldi		int_tmp,SB				;load stopbits
		cp		rx_bitcnt,int_tmp		;all except stopbits received?
		brne	isr_cm1a_exit			;no -> exit
		sbr		uart_stat,1<<rx_ready	;yes -> update rx_ready, ready for next byte
		mov		rx_data,rx_shift		;store byte
		rjmp	isr_cm1a_exit
isr_cm1a_stop:
		in		int_tmp,TIMSK			;disable timer1 output compare1a interrupt
		andi	int_tmp,~(1<<OCIE1A)
		out		TIMSK,int_tmp
		ldi		int_tmp,1<<PCIF			;clear PCI interrupt flag
		out		GIFR,int_tmp
		in		int_tmp,GIMSK			;global enable PCI
		ori		int_tmp,1<<PCIE
		out		GIMSK,int_tmp
isr_cm1a_exit:
		out		SREG,int_sr
		reti

;transmit bit interrupt
ISR_COMP1B:
		in		int_sr,SREG
		sec								;set carry for stopbits
		ror 	tx_shift				;shift bit into carry
		brcs	isr_cm1b_out1
		cbi		FD_PORT,TxD				;send 0
		rjmp	isr_cm1b_out0
isr_cm1b_out1:
		sbi		FD_PORT,TxD				;send 1
isr_cm1b_out0:
		dec 	tx_bitcnt				;last bit?
		breq	isr_cm1b_stop			;yes
		ldi		int_tmp,SB				;load stopbits
		cp		tx_bitcnt,int_tmp		;all except stopbits received?
		brne	isr_cm1b_exit			;no -> exit
		cbr		uart_stat,1<<tx_busy	;yes -> update tx_busy, ready for next byte
		rjmp	isr_cm1b_exit
isr_cm1b_stop:
		in		int_tmp,TIMSK			;disable timer1 output compare1b interrupt
		andi	int_tmp,~(1<<OCIE1B)
		out		TIMSK,int_tmp
isr_cm1b_exit:
		out		SREG,int_sr
		reti

;init pins timer and interrupts
fd_uart_init:
		push	tmp0					;save register
		clr		uart_stat				;init status
		cbi		FD_PORT,RxD				;RxD -> no pullup
		sbi		FD_PORT,TxD				;TxD -> pullup
		cbi		FD_DDR,RxD				;RxD -> input
		sbi		FD_DDR,TxD				;TxD -> output
		ldi		tmp0,T1TOP				;max value -> clear timer1
		out		OCR1C,tmp0
		ldi		tmp0,T1INIT				;timer1 clock = CSXX, clear on compare match OCR1C
		out		TCCR1,tmp0				;start timer1
		ldi		tmp0,1<<FD_PCINT		;enable Pin-Change-Interrupt on PCINTX
		out		PCMSK,tmp0
		in		tmp0,GIMSK				;global enable PCI
		ori		tmp0,1<<PCIE
		out		GIMSK,tmp0
		pop		tmp0
		ret

;put char in transmit shift register, start sender
fd_uart_putchar:
		push	tmp0					;save register
		in		tmp0,TCNT1				;load actual timer1 for OCR1B
		out		OCR1B,tmp0
		sbr		uart_stat,1<<tx_busy	;signal tx_busy
		mov		tx_shift,tx_data		;move data to shift register
		ldi		tmp0,1+8+SB				;1 startbit, 8 data, SB stopbits
		mov		tx_bitcnt,tmp0
		ldi		tmp0,1<<OCF1B			;clear output compare1b flag
		out		TIFR,tmp0
		cli
		in		tmp0,TIMSK				;enable timer1 output compare1b interrupt
		ori		tmp0,1<<OCIE1B
		out		TIMSK,tmp0
		sei
		cbi		FD_PORT,TxD				;send 0 -> start bit
		pop		tmp0
		ret

;wait for empty transmitter, then putchar
fd_uart_putchar_wait:
		push	tmp0					;save register
fd_uart_putchar_wait_busy:
		sbrc	uart_stat,tx_busy		;wait while tx_busy
		rjmp	fd_uart_putchar_wait_busy
		ldi		tmp0,30					;small delay for putchar to complete
fd_uart_put_wait_loop:		
		dec		tmp0
		brne	fd_uart_put_wait_loop
		rcall	fd_uart_putchar
		pop		tmp0
		ret

;output byte as hex
fd_uart_putchar_hex:
		push	tmp0					;save register
		push	tx_data					;save byte to send
		ldi		tmp0,'0'
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ldi		tmp0,'x'
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		pop		tmp0					;restore byte
		push	tmp0					;save for second nibble
		swap	tmp0
		rcall	fd_uart_putchar_hex_out
		pop		tmp0					;restore byte
		rcall	fd_uart_putchar_hex_out
		pop		tmp0
		ret
fd_uart_putchar_hex_out:
		andi	tmp0,0x0f
		cpi		tmp0,10
		brlo	fd_uart_putchar_hex_num
		subi	tmp0,-('a'-10)
		rjmp	fd_uart_putchar_hex_alpha
fd_uart_putchar_hex_num:
		subi	tmp0,-'0'
fd_uart_putchar_hex_alpha:
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ret

;output enter
fd_uart_put_enter:
		push	tmp0					;save register
		ldi		tmp0,13
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		ldi		tmp0,10
		mov		tx_data,tmp0
		rcall	fd_uart_putchar_wait
		pop		tmp0
		ret

;put string in pgm memory (Z)
fd_uart_put_PGM_str:
		lpm		tx_data,Z+				;load char
		tst		tx_data					;string terminated?
		breq	fd_uart_put_exit
		rcall	fd_uart_putchar_wait
		rjmp	fd_uart_put_PGM_str
fd_uart_put_exit:		
		ret


uart_soft_fd_cseg_end:						;cseg end address

.org	PCI0addr
		rjmp	ISR_PCINT0				;PCINT0
.org	OC1Aaddr
		rjmp	ISR_COMP1A				;TIM1_COMPA
.org	OC1Baddr
		rjmp	ISR_COMP1B				;TIM1_COMPB
