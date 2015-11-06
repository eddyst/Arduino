/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ow_exbo.c´                       */
/*                   Header-Datei speziell zum PT-1-Wire-ExBo                               */
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
/*     Datei:             ow_exbo.h                                                         */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ow_exbo_H_
  
  #define  _ow_exbo_H_

        // Gloable Variablen



        /****************************/
        /*** Funktions-Prototypen ***/
        /****************************/

        /*** PT-1-Wire-ExBo-Funktion: gezielt einen Port-Pin setzen/zurücksetzen ohne die ***/
        /*** anderen Port-Pins zu beeinflussen                                            ***/
        /*** Besonderheit wg. Taster an den Pins PIO0 - PIO3                              ***/
        extern void ow_exbo_set_port(unsigned char nr,unsigned char zustand);


        /*** PT-1-Wire-ExBo-Funktion: Relais ein- und ausschalten ***/
        extern void ow_exbo_rel(unsigned char nr, unsigned char zustand);


        /*** PT-1-Wire-ExBo-Funktion: Summer ein- und ausschalten ***/
        extern void ow_exbo_sum(unsigned char zustand);


        /*** PT-1-Wire-ExBo-Funktion: Scannen der Tasten auf dem ExBo ***/
        extern unsigned char ow_exbo_ta_in(void);


        /* PT-1-Wire-ExBo-Funktion:  Scannt die 3 Tasten vom LCD-Modul ***/
        extern unsigned char ow_exbo_lcd_ta_in(void);


#endif
