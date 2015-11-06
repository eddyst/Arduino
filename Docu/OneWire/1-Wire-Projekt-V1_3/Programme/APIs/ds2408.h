/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ds2408.c´                        */
/*                   Header-Datei zum 1-Wire-Chip: DS2408 = 8-fach dig. I/O (PIO)           */
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
/*     Letzte Änderung:   27.07.2010                                                        */
/*                                                                                          */
/*     Datei:             ds2408.h                                                          */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ds2408_H_
  
  #define  _ds2408_H_

        // Gloable Variablen



        /****************************/
        /*** Funktions-Prototypen ***/
        /****************************/

        /*** Init. des DS2408er: Einschreiben eines Byte ins Control/Status-Reg ***/
        extern void ds2408_init(unsigned char dat);


        /*** Schreiben eines Bytes (8 Bits) in den DS2408er ***/
        extern void ds2408_wr_byte(unsigned char dat);


        /*** Einlesen der aktuellen Zustände an den PIO-Port-Pins ***/
        extern unsigned char ds2408_rd_zustand(void);


        /*** Setzen / Rücksetzen eines einzelnen PIO-Port-Pins ***/
        extern void ds2408_set_port(unsigned char nr, unsigned char zustand);


        /*** Einlesen des Zustandes eines einzelnen Port-Pins ***/
        extern unsigned char ds2408_get_port(unsigned char nr);


        /*** Einlesen der aktuellen Zustände der Flanken-Wechsel-FFs der PIO-Port-Pins ***/
        extern unsigned char ds2408_rd_fla(void);


        /*** Reset der Flanken-Wechsel FFs des DS2408er (Reg. 8ah = PIO Activity Latch State) ***/
        extern void ds2408_fla_cl(void);


        /*** Einlesen des Zustandes eines einzelnen Flanken-Wechsel-FF eines Port-Pins ***/
        extern unsigned char ds2408_get_fla(unsigned char nr);


#endif
