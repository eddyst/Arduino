/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ds2408_lcd.c´                    */
/*                   Header-Datei zum Betrieb eines LC-Displays am DS2408er                 */
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
/*     Datei:             ds2408_lcd.h                                                      */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ds2408_lcd_H_
  
  #define  _ds2408_lcd_H_

        // Gloable Variablen



        /****************************/
        /*** Funktions-Prototypen ***/
        /****************************/

        /*** Ein Byte, gesplittet in zwei Niibles, in ein LCD-Register einschreiben ***/
        extern void ds2408_lcd_wr(unsigned char dat, unsigned char reg);


        /*** LCD initialisieren im 4 (!)-Bit-Modus ***/
        extern void ds2408_lcd_init(void);


        /*** LCD löschen ***/
        extern void ds2408_lcd_clear(void);


        /*** LCD ausschalten, Daten bleiben jedoch erhalten ***/
        extern void ds2408_lcd_off(void);


        /*** LCD einschalten, mit/ohne Cursor ***/
        extern void ds2408_lcd_on(unsigned char n);


        /*** Cursor auf LCD positionieren: Zeile,Spalte ***/
        extern void ds2408_lcd_csr_set(unsigned char z, unsigned char s);


        /*** Zeichen an der aktuellen Cursor-Position ausgeben ***/
        extern void ds2408_lcd_dis_zei(unsigned char zeichen);


        /*** Zeichen an übergebener Cursor-Position ausgeben ***/
        extern void ds2408_lcd_dis_csr_zei(unsigned char z, unsigned char s, unsigned char c);


        /*** Text (String) ab Cursor-Position darstellen ***/
        extern void ds2408_lcd_dis_txt(unsigned char z, unsigned char s, unsigned char *a);


#endif
