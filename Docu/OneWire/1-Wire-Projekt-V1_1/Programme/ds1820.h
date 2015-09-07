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
/*     Letzte Änderung:   12.02.2010                                                        */
/*                                                                                          */
/*     Datei:             ds1820.h                                                          */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ds1820_H_
  
  #define  _ds1820_H_


        // Gloable Variablen
        unsigned char ds1820[9];            // Array für empfangene Daten vom DS1820er: max 9 Bytes möglich


        // Matrix für bis zu 5 angeschlossenen DS18S20er-Slaves:
        //   -  Stations-Nummern:  0 - 4
        //   -  Jeder Slave hat einen 8 Byte großen ROM-Code.
        //   -  Jede Zeile in der Matrix entspricht dem ROM-Code eines Slaves.
        //   -  Eintragungen fangen beim CRC-Code an, letztes Byte: Family-Code.
        code unsigned char ds1820_mat[5][8] = {
        {0x3f,0x00,0x08,0x01,0xdb,0x13,0x6c,0x10},
        {0,0,0,0,0,0,0,0},
        {0x0c,0x00,0x08,0x01,0xdb,0x08,0x21,0x10},
        {0,0,0,0,0,0,0,0},
        {0x54,0x00,0x08,0x01,0x86,0x70,0x0a,0x10}
        };



        /****************************/
        /*** Funktions-Prototypen ***/
        /****************************/

        /*** Abfrage eines DS18S20ers mit der Slave-Adresse s_adr ***/
        extern void ds1820_rd_temp(unsigned char s_adr);

        /*** Übertragung von 2 Bytes in die Register TH und TL des Scratch Pads ***/
        extern void ds1820_wr_t_reg(unsigned char TH, unsigned char TL);



#endif
