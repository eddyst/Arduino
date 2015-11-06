/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Header-Datei zur C-Source-Code-Datei ´ds2450.c´                        */
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
/*     Datei:             ds2450.h                                                          */
/*                                                                                          */
/********************************************************************************************/

#ifndef  _ds2450_H_
  
  #define  _ds2450_H_


    /*******************************************************************************************/
    /*** Globale Variablen *********************************************************************/
    /*******************************************************************************************/
    
    /*** Unterschiedliche Initialisierungs-Vektoren für den DS2450er                     ***/
    /*** Ablage der Werte im Programmspeicher-Bereich !                                  ***/
    /*** Aufbau: Aufl. Kanal A, Aufl. Kanal B, Aufl. Kanal C, Aufl. Kanal D              ***/
    /***         Ref.Spg. Kanal A, Ref.Spg. Kanal B, Ref.Spg, Kanal C, Ref.Spg. Kanal D  ***/
    code unsigned char ds2450_init_v0[8] = {8,8,8,8,1,1,1,1};
    code unsigned char ds2450_init_v1[8] = {8,8,10,10,1,1,1,1};
    code unsigned char ds2450_init_v2[8] = {10,10,10,10,1,1,1,1};
    code unsigned char ds2450_init_v3[8] = {2,3,4,5,1,1,1,1};
    code unsigned char ds2450_init_v4[8] = {16,16,16,16,1,1,1,1};
    code unsigned char ds2450_init_v5[8] = {8,8,8,8,0,0,0,0};
    
    // Auflösungewerte (2er-Potenzen)
    code unsigned long pot2[17] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536};
    
    // Variablen zum Abspeichern der Kanal-Auflösung
    unsigned char res_A,res_B,res_C,res_D;
    
    // Variablen zum Abspeichern der Kanal-Referenzspg.
    float refspg_A,refspg_B,refspg_C,refspg_D;
    
    // Array, um die 8 Byte DS2450er-Page-Daten abzuspeichern: Sende/Empfangs-Richtung
    unsigned char ds2450_p[8];
    
    // Abspeicherung des 16-Bit-CRC-Wertes, zerlegt in zwei Bytes
    unsigned char crc1,crc2;
    
    
    /****************************/
    /*** Funktions-Prototypen ***/
    /****************************/

    /*** Schreiben von 8 Byte-Daten in eine DS2450er-Page; sinnvoll nur für Page 1 und 2 ***/
    /*** Inhalt des 8-Byte-Arrays ds2450_p[.] wird in die angegebene Page geschrieben.   ***/
    /*** LSByte der Page aus der Array-Stelle 0, usw.                                    ***/
    /*** p_nr = 1 oder 2                                                                 ***/
    extern void ds2450_write_page(unsigned char p_nr);


    /*** Auslesen von 8 Byte-Daten aus einer DS2450er-Page, für Pages  0 - 3 möglich.    ***/
    /*** Ausgelesene Bytes werden in 8-Byte-Array ds2450_p[.] abgespeichert und zwar:    ***/
    /*** LSByte der Page in Array-Stelle 0, usw.                                         ***/
    /*** p_nr = 0, 1, 2, 3                                                               ***/
    extern void ds2450_read_page(unsigned char p_nr);


    /*** Initialisierung des DS2450ers durch Übergabe u. Auswertung des entspr. Init-Vektors ***/
    /*** Der gewünschte Init-Vektor wird an die Fkt. übergeben                               ***/
    extern void ds2450_init(unsigned char *v);


    /*** Start der Wandlung und Auslesen für alle 4 Kanäle eines einzelnen DS2450ers am Bus ***/
    extern void ds2450_convert_all(void);


#endif
