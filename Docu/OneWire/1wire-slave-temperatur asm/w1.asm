/**
 * complete 1wire master implementation
 * 
 * All functions needed for a 1wire master. The number of managed 1wire roms is
 * limited by the amount of RAM. The pullup from the PORT I/O pin is used. For
 * parasite powered devices an additional pullup resistor may needed.
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
.def	w1_stat		= R17
#ifndef	TMP_REGS
#define	TMP_REGS
.def	tmp0		= R24
.def	tmp1		= R25
#endif


;port and pin definitions
.equ	W1			= PB4					;w1 pin = PB4
.equ	W1_PORT		= PORTB
.equ	W1_DDR		= DDRB
.equ	W1_PIN		= PINB


;general 1wire ROM commands
.equ	MATCH_ROM	= 0x55
.equ	SKIP_ROM	= 0xCC
.equ	SEARCH_ROM	= 0xF0


;constants
.equ	W1_SB0		= 0						;search response bits
.equ	W1_SB1		= 1
.equ	W1_PRES_ERR	= 2						;errors
.equ	W1_SC_ERR	= 3
.equ	W1_DATA_ERR	= 4
.equ	W1_CRC_ERR	= 5

.equ	MAX_ROMS	= 8



.dseg
w1_sseq:	.byte	8
snr_rom:	.byte	8 * MAX_ROMS
romcount:	.byte	1



.cseg										;Code-Segment

.include	"delay.asm"


;init io pins and output master reset pulse
w1_init:
		push	tmp0						;save registers
		push	tmp1
		clr		w1_stat
		sbi		W1_PORT,W1					;pullup enable
		set									;signal masterreset
		rcall	w1_reset
		ldi		tmp1,HIGH(4000)				;4ms pause after master reset
		ldi		tmp0,LOW(4000)				;for parasite powered devices to load
		rcall	delay_us
		clt
		rcall	w1_reset					;reset again because of double presence pulse after master reset
		clr		tmp0						;init rom number
		sts		romcount,tmp0
		pop		tmp1
		pop		tmp0
		ret


;compute crc of data in Y, number of bytes in tmp1, return tmp0
w1_crc:
		push	ZL
		push	ZH
		push	tmp0
		clr		tmp0
		sts		crc8_data,tmp0				;init crc8
w1_crc_loop:
		ld		tmp0,Y+						;load data byte
		rcall	crc8						;compute crc
		dec		tmp1
		brne	w1_crc_loop
		pop		tmp0
		pop		ZH
		pop		ZL
		ret

;reset w1 bus
w1_reset:
		push	tmp0						;save registers
		push	tmp1
		cbr		w1_stat,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)
		cbi		W1_PORT,W1					;disable pullup
		sbi		W1_DDR,W1					;output 0
		ldi		tmp1,210					;210x480us ~ 100ms master reset
w1_mreset_start:
		push	tmp1
		ldi		tmp1,HIGH(480)
		ldi		tmp0,LOW(480)
		rcall	delay_us
		pop		tmp1
		brtc	w1_mreset_end				;kein masterreset
		subi	tmp1,1						;loop for tmp1 count
		brne	w1_mreset_start
w1_mreset_end:
		sbi		W1_PORT,W1					;pullup enable
		cbi		W1_DDR,W1					;reset release -> output 1
		ldi		tmp1,HIGH(75)
		ldi		tmp0,LOW(75)
		rcall	delay_us
		sbic	W1_PIN,W1					;sample presence detect
		ori		w1_stat,1<<W1_PRES_ERR
		ldi		tmp1,HIGH(480-75)
		ldi		tmp0,LOW(480-75)
		rcall	delay_us
		sbis	W1_PIN,W1					;short circuit detect
		ori		w1_stat,1<<W1_SC_ERR
		pop		tmp1
		pop		tmp0
		ret

;output bit in T-flag, return bit in T-flag
w1_bit_io:
		push	tmp0						;save registers
		push	tmp1
#ifdef	PROJ_1WIRE_SLAVE_TEMPERATURE
/* special check that timer for multiplex display isnt short before expiring */
w1_bit_io_wait_timer:
		lds		tmp0,timer_1ms
		cpi		tmp0,4
		brsh	w1_bit_io_wait_timer
/* special check for project 1wire-slave-temperatur end */
#endif
		cbi		W1_PORT,W1					;disable pullup
		sbi		W1_DDR,W1					;activate output -> output 0 -> start sequence
		clr		tmp1						;3us start signal
		ldi		tmp0,3
		rcall	delay_us
		brtc	w1_bit_out0					;output bit -> T
		sbi		W1_PORT,W1					;pullup enable
		cbi		W1_DDR,W1					;deactivate output -> send 1
w1_bit_out0:
		clr		tmp1						;sample afer 3+10us
		ldi		tmp0,10
		rcall	delay_us
		sbis	W1_PIN,W1					;sample bit (0)
		clt
		sbic	W1_PIN,W1					;sample bit (1)
		set
		clr		tmp1						;remain output until 60us reached
		ldi		tmp0,60-3-10
		rcall	delay_us
		sbi		W1_PORT,W1					;pullup enable
		cbi		W1_DDR,W1					;deactivate output
		clr		tmp1						;1us recovery time
		ldi		tmp0,1
		rcall	delay_us
		pop		tmp1
		pop		tmp0
		ret

;output byte in tmp0, result in tmp0
w1_byte_io:
		push	tmp1						;save registers
		ldi		tmp1,8						;init counter
w1_byte_loop:
		bst		tmp0,0
		push	tmp0
		push	tmp1
		rcall	w1_bit_io
		pop		tmp1
		pop		tmp0
		clc
		brtc	w1_byte_in0					;read 0 bit
		sec
w1_byte_in0:
		ror		tmp0						;shift received bit into byte
		dec		tmp1
		brne	w1_byte_loop
		pop		tmp1
		ret

;turn on strong pullup -> parasite power
w1_parasite_on:
		sbi		W1_PORT,W1					;pullup enable
		sbi		W1_DDR,W1					;activate output
		ret

;turn off strong pullup -> parasite power off
w1_parasite_off:
		sbi		W1_PORT,W1					;pullup enable
		cbi		W1_DDR,W1					;deactivate output
		ret

;search 1 rom, save at Y address, X = pointer sseq
w1_search_rom:
		push	tmp0						;save registers
		push	tmp1
		clt
		rcall	w1_reset					;start search with reset
		ldi		tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)
		and		tmp0,w1_stat				;check for errors
		brne	w1_search_exit
		ldi		tmp0,SEARCH_ROM
		rcall	w1_byte_io
		ldi		tmp1,8						;init byte counter
w1_search_byte_loop:
		push	tmp1
		ldi		tmp1,0x80					;init bit counter, shift to count
w1_search_bit_loop:
		push	tmp1
		set									;write 1 to read bit
		rcall	w1_bit_io
		bld		w1_stat,W1_SB0				;store rom response bit 0
		set									;write 1 to read bit
		rcall	w1_bit_io
		bld		w1_stat,W1_SB1				;store rom response bit 1
		mov		tmp0,w1_stat				;test bits
		andi	tmp0,3
		breq	w1_search_multi
		cpi		tmp0,3						;both bits = 1 -> data error
		brne	w1_search_single
		ori		w1_stat,1<<W1_DATA_ERR
		pop		tmp1						;clean stack
		pop		tmp1
		rjmp	w1_search_exit
w1_search_single:
		ld		tmp0,X						;load sseq
		pop		tmp1						;load bit counter
		com		tmp1
		and		tmp0,tmp1					;clear bit in sseq
		com		tmp1
		st		X,tmp0						;save sseq
		bst		w1_stat,W1_SB0				;write back bit to select rom
		rjmp	w1_search_save
w1_search_multi:
		ld		tmp0,X						;load sseq
		pop		tmp1						;load bit counter
		and		tmp0,tmp1					;select what to write back
		clt
		brne	w1_search_save
		set
w1_search_save:
		push	tmp1						;save bit counter
		ld		tmp0,Y						;first store bit
		clc
		brtc	w1_search_in0				;read 0 bit
		sec
w1_search_in0:
		ror		tmp0						;shift received bit into byte
		st		Y,tmp0						;store byte
		rcall	w1_bit_io					;write select rom bit back
		pop		tmp1
		clc
		ror		tmp1						;dec inner loop counter
		brne	w1_search_bit_loop
		adiw	YL,1						;inc rom pointer
		adiw	XL,1						;inc search bits pointer
		pop		tmp1
		dec		tmp1
		brne	w1_search_byte_loop
w1_search_exit:
		pop		tmp1
		pop		tmp0
		ret

;search all roms, save at address snr_rom
w1_search_all_roms:
		push	tmp0						;save registers
		push	tmp1
		push	XL
		push	XH
		push	YL
		push	YH
		ldi		YL,LOW(snr_rom)				;load rom storage address
		ldi		YH,HIGH(snr_rom)
		ldi		XL,LOW(w1_sseq)				;init search sequence
		ldi		XH,HIGH(w1_sseq)
		ldi		tmp1,8						;loop counter
		ser		tmp0
w1_sseq_init:
		st		X+,tmp0						;store 0xFFFFFFFFFFFFFFFF
		dec		tmp1
		brne	w1_sseq_init
		rjmp	w1_sseq_start
w1_sseq_continue:
		push	tmp0						;save complete mask
		clr		tmp1
w1_sseq_update:
		sec
		rol		tmp1						;set all high bits to 1...
		clc
		ror		tmp0						;until find lowest 1 bit in tmp0
		brcc	w1_sseq_update
		pop		tmp0
		eor		tmp1,tmp0
		st		X,tmp1						;store actual sseq
w1_sseq_start:
		ldi		XL,LOW(w1_sseq)				;init search sequence
		ldi		XH,HIGH(w1_sseq)
		rcall	w1_search_rom				;search 1 wire roms
		ldi		tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)
		and		tmp0,w1_stat				;check for errors
		brne	w1_sseq_exit				;no rom found, exit
		subi	YL,8						;set pointer to found rom start address
		sbci	YH,0
		ldi		tmp1,7						;7 byte data
		rcall	w1_crc						;compute crc
		lds		tmp0,crc8_data				;load computed crc
		ld		tmp1,Y+						;load crc byte
		cp		tmp0,tmp1					;correct?
		breq	w1_sseq_crc_ok
		ori		w1_stat,1<<W1_CRC_ERR		;no, set error flag
		subi	YL,8						;set pointer back to last rom
		sbci	YH,0
		rjmp	w1_sseq_crc_fail
w1_sseq_crc_ok:
		lds		tmp0,romcount				;inc number of roms
		inc		tmp0
		sts		romcount,tmp0
		cpi		tmp0,MAX_ROMS				;maximum number roms found -> exit
		breq	w1_sseq_exit
w1_sseq_crc_fail:
		ldi		tmp1,8						;update search sequence loop counter
w1_sseq_check:
		ld		tmp0,-X
		tst		tmp0						;all bits clear?
		brne	w1_sseq_continue			;no -> search again
		ser		tmp0						;set all leading bits to 1 for next search
		st		X,tmp0
		dec		tmp1						;dec loop counter
		brne	w1_sseq_check
w1_sseq_exit:
		pop		YH
		pop		YL
		pop		XH
		pop		XL
		pop		tmp1
		pop		tmp0
		ret

;send w1 command, Y: pointer to rom, tmp0: command
w1_command:
		push	tmp0						;save registers
		push	tmp1
		push	YL
		push	YH
		push	tmp0						;save command
		clt
		rcall	w1_reset					;start command with reset
		ldi		tmp0,(1<<W1_PRES_ERR)|(1<<W1_SC_ERR)
		and		tmp0,w1_stat				;check for errors
		breq	w1_cmd_start
		pop		tmp0						;clean stack and exit
		rjmp	w1_cmd_exit
w1_cmd_start:
		tst		YL							;rom pointer?
		brne	w1_cmd_match_rom			;yes -> send MATCH_ROM and id
		tst		YH							;rom pointer?
		brne	w1_cmd_match_rom			;yes -> send MATCH_ROM and id
		ldi		tmp0,SKIP_ROM				;no -> send SKIP_ROM
		rcall	w1_byte_io
		rjmp	w1_cmd_send
w1_cmd_match_rom:
		ldi		tmp0,MATCH_ROM				;send MATCH_ROM
		rcall	w1_byte_io
		ldi		tmp1,8
w1_cmd_match_loop:
		push	tmp1						;save loop counter
		ld		tmp0,Y+						;load rom id
		rcall	w1_byte_io
		pop		tmp1
		dec		tmp1						;dec loop counter
		brne	w1_cmd_match_loop
w1_cmd_send:
		pop		tmp0						;restore command
		rcall	w1_byte_io					;send out
w1_cmd_exit:
		pop		YH
		pop		YL
		pop		tmp1
		pop		tmp0
		ret

;find w1 rom, T-flag: start new search, tmp0: id[0] to find, return Y -> pointer to rom, NULL -> notfound
w1_find_rom_type:
		push	tmp0						;save registers
		push	tmp1
		brtc	w1_find_rom_type_cont
		ldi		YL,LOW(snr_rom)				;load rom storage address
		ldi		YH,HIGH(snr_rom)
		lds		tmp1,romcount				;roms existent?
		rjmp	w1_find_rom_type_start
w1_find_rom_type_cont:
		dec		tmp1
		adiw	YL,8						;next rom
w1_find_rom_type_start:
		tst		tmp1
		breq	w1_find_rom_type_exit_null	;no roms in list
		push	tmp1						;save loop counter
		ld		tmp1,Y
		cp		tmp0,tmp1
		pop		tmp1						;restore loop counter
		breq	w1_find_rom_type_exit		;rom type found
		rjmp	w1_find_rom_type_cont
w1_find_rom_type_exit_null:
		clr		YL							;return value not found
		clr		YH
w1_find_rom_type_exit:
		pop		tmp1
		pop		tmp0
		ret



w1_cseg_end:								;cseg end address
