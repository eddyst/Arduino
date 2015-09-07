/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ow.c´                            */
/*                                                                                          */
/*                                                                                          */
/*     Autoren:           P.G. / B.v.B.                                                     */
/*                                                                                          */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)                 */
/*                                                                                          */
/*                                                                                          */
/*                                                                                          */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                              */
/*                                                                                          */
/*     Letzte Änderung:   12.02.2010                                                        */
/*                                                                                          */
/*     Datei:             ow.h                                                              */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ow_H_
  
  #define  _ow_H_

        // Port-Pin zur Realisierung des 1-Wire-Busses
        unsigned char bit DQ  @  0xb4;      // Port-Pin P3.4


        // Gloable Variablen
        unsigned char rom_c[8];             // Array zur Aufnahme des ROM-Codes


        /****************************/
	    /*** Funktions-Prototypen ***/
        /****************************/

        /*** Realisierung des 1-Wire-Delays ***/
        extern void ow_delay(unsigned char delay);

        /*** 1-Wire-Reset ***/
        extern unsigned char ow_reset(void);

        /*** Ausgabe eines Bits über den 1-Wire-Bus ***/
        extern void ow_wr_bit(unsigned char bitwert);

        /*** Einlesen eines Bits über den 1-Wire-Bus ***/
        extern unsigned char ow_rd_bit(void);

        /*** Ausgabe eines Bytes über den 1-Wire-Bus ***/
        extern void ow_wr_byte(unsigned char dat);

        /*** Einlesen eines Bytes über den 1-Wire-Bus ***/
        extern unsigned char ow_rd_byte(void);

        /*** Lesen des 64-Bit-ROM-Identifiers ***/
        extern void ow_rd_rom(void);

#endif
