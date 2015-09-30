/**
 * delay loops for exact delays
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



#ifndef	delay_asm
#define	delay_asm

#ifndef	TMP_REGS
#define	TMP_REGS
.def	tmp0		= R24
.def	tmp1		= R25
#endif


;exactly 20 cycles (1us) per count in tmp1:tmp0 + 2 cycles ret
delay_us:
		nop
		nop
delay_us_inloop:
		push	tmp1
		ldi		tmp1,3
delay_loop:
		subi	tmp1,1
		brne	delay_loop
		pop		tmp1
		subi	tmp0,1
		sbci	tmp1,0
		tst		tmp0
		brne	delay_us
		tst		tmp1
		brne	delay_us_inloop
delay_us_exit:
		ret


#endif
