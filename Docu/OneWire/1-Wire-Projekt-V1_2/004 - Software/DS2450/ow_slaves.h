/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei mit den 1-Wire-Slave-Adressen (ID-Identifier)             */
/*                   der im System eingesetzten 1-Wire-Slaves                               */
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
/*     Letzte Änderung:   12.05.2010                                                        */
/*                                                                                          */
/*     Datei:             ow_slaves.h                                                       */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ow_slaves_H_
  
  #define  _ow_slaves_H_

        // Diese Variable enthält im Hauptprogramm die Zeilennummer des gerade aktiven Salves,
        // d.h. die Nummer desjenigen Salves, mit dem gerade gearbeitet wird.
        unsigned char ow_sl_aktiv;

        // Matrix für bis zu 20 angeschlossene 1-Wire-Slaves:
        //   -  Jeder Slave hat einen 8 Byte großen ROM-Code (ROM Identifier).
        //   -  Jede Zeile in der Matrix entspricht dem ROM-Code eines Slaves.
        //   -  Eintragungen in einer Zeile fangen beim CRC-Code an, letztes Byte: Family-Code.
        //   -  Die Slave-Zeilen können in beliebiger Reihenfolge stehen.
        //   -  Die jeweilige Zeilennummer entspricht der Nummer, unter der der Slave 
        //      im Programm  angesprochen wird = Wert der Variablen ´ow_sl_aktiv´

        code unsigned char ow_sl_mat[20][8] = {
        {0,0,0,0,0,0,0,0},                              // Slave 0:
        {0xfe,0x00,0x08,0x01,0x9c,0x0e,0x6d,0x10},      // Slave 1:  DS18S20er - 1
        {0x0c,0x00,0x08,0x01,0xdb,0x08,0x21,0x10},      // Slave 2:  DS18S20er - 2
        {0xc6,0x00,0x08,0x01,0x9c,0x3f,0x0b,0x10},      // Slave 3:  DS18S20er - 3
        {0x54,0x00,0x08,0x01,0x86,0x70,0x0a,0x10},      // Slave 4:  DS18S20er - 4
        {0,0,0,0,0,0,0,0},                              // Salve 5:
        {0,0,0,0,0,0,0,0},                              // Salve 6:
        {0,0,0,0,0,0,0,0},                              // Slave 7:
        {0,0,0,0,0,0,0,0},                              // Slave 8:
        {0,0,0,0,0,0,0,0},                              // Slave 9:
        {0xd9,0x00,0x00,0x00,0x13,0xfa,0xd3,0x20}       // Slave 10:  DS2450er - 0
       };

#endif
