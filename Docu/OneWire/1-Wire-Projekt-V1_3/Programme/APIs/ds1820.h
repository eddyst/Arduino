/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ds1820.c´                        */
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
/*     Letzte Änderung:   03.09.2010                                                        */
/*                                                                                          */
/*     Datei:             ds1820.h                                                          */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ds1820_H_
  
  #define  _ds1820_H_


        // Gloable Variablen
        unsigned char ds1820[9];            // Array für empfangene Daten vom DS1820er: max 9 Bytes möglich


        /****************************/
        /*** Funktions-Prototypen ***/
        /****************************/

        /*** Abfrage eines DS18S20ers ***/
        extern float ds1820_rd_temp(void);

        /*** Übertragung von 2 Bytes in die Register TH und TL des Scratch Pads ***/
        extern void ds1820_wr_t_reg(unsigned char TH, unsigned char TL);


#endif
