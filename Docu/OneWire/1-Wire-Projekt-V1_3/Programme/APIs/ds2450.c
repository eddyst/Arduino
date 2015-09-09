/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus - DS2450:                                                                 */
/*     =====================                                                                */
/*                                                                                          */
/*     Sammlung der API-Funktionen zum Betrieb des 4-Kanal-A/D-Wandlers DS2450              */
/*                                                                                          */
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
/*     Letzte Änderung:   07.06.2010                                                        */
/*                                                                                          */
/*     Datei:             ds2450.c                                                          */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ds2450.h"
#include "ow.h"   // 1-Wire-API


/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Schreiben von 8 Byte-Daten in eine DS2450er-Page; sinnvoll nur für Page 1 und 2 ***/
/*** Inhalt des 8-Byte-Arrays ds2450_p[.] wird in die angegebene Page geschrieben.   ***/
/*** LSByte der Page aus der Array-Stelle 0, usw.                                    ***/
/*** p_nr = 1 oder 2                                                                 ***/

void ds2450_write_page(unsigned char p_nr)
{
    unsigned char i,zw;

    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(20);

    /*********************************************/
    /*** Werte in übergebene Page einschreiben ***/
    /*********************************************/

    // Adressierung des jeweils als aktiv ausgewählten Slaves
    ow_match();

    // Schreib-Befehl senden: "WRITE Memory" = 0x55
    ow_wr_byte(0x55);

    // Page-Adresse übermitteln
    ow_wr_byte(p_nr*8);             // TA1 = niederwertig = 08h, 10h
    ow_wr_byte(0x00);               // TA2 = höherwertig  = 00 (immer fest)

    // Die 8 Bytes aus dem Vektor ds2450_p[.] für die Page aussenden
    for (i=0; i<8; i++)
    {
        // Byte senden
        ow_wr_byte(ds2450_p[i]);

        // 16-Bit-CRC vom DS2450er zurück lesen
        crc1=ow_rd_byte();              // CRC-1
        crc2=ow_rd_byte();              // CRC-2

        // Kopie des gesendeten Datums vom DS2450er zurücklesen und vergleichen
        zw=ow_rd_byte();
        if (zw != ds2450_p[i])          // Fehler
        {
            ow_fem(21);     // Fehlermeldung ausgeben
        }

        // CRC-Überprüfung; entfällt zur Zeit !!

    }

    // Fertig mit dem Senden einer Page:
    // Master sendet abschließenden Reset-Impuls
    ow_pres_check(22);
}

/********************************************************************************************/

/*** Auslesen von 8 Byte-Daten aus einer DS2450er-Page, für Pages  0 - 3 möglich.    ***/
/*** Ausgelesene Bytes werden in 8-Byte-Array ds2450_p[.] abgespeichert und zwar:    ***/
/*** LSByte der Page in Array-Stelle 0, usw.                                         ***/
/*** p_nr = 0, 1, 2, 3                                                               ***/

void ds2450_read_page(unsigned char p_nr)
{
    unsigned char i;

    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(23);

    /************************************/
    /*** Read Memory  ***/
    /************************************/

    // Adressierung des jeweils als aktiv ausgewählten Slaves
    ow_match();

    // Speicher lesen-Befehl senden: "Read Memory" = 0xaa
    ow_wr_byte(0xaa);

    // Register-Start-Adresse übermitteln: TA1 und TA2
    ow_wr_byte(p_nr*8);         // TA1 = niederwertig = 00h, 08h, 10h, 18h
    ow_wr_byte(0x00);           // TA2 = höherwertig  = immer 00

    // Auslesen von 8 Byte hintereinander aus DS2450er
    // = eine Page auslesen !
    for (i=0; i<8; i++)
    {
        ds2450_p[i] = ow_rd_byte();
    }

    // Da hier End of page:
    // 16-Bit-CRC vom DS2450er zurück lesen
    crc1=ow_rd_byte();            // CRC-1
    crc2=ow_rd_byte();            // CRC-2

    // Abschließender Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(24);

    // CRC-Überprüfung; entfällt zur Zeit !!

}

/*********************************************************************************************/

/*** Initialisierung des DS2450ers durch Übergabe u. Auswertung des entspr. Init-Vektors ***/
/*** Der gewünschte Init-Vektor wird an die Fkt. übergeben                               ***/

void ds2450_init(unsigned char *v)
{
    unsigned char zw;

    /************************************************************/
    /*** Festlegungen für Page 1 in Sende-Vektor einschreiben ***/
    /************************************************************/
    /*** Auflösungen ***/
    // Kanal A - Adr: 08h
    res_A = v[0];                           // Abspeichern der Kanal-Auflösung
    if (v[0] == 16) ds2450_p[0] = 0;
               else ds2450_p[0] = v[0];

    // Kanal B - Adr: 0ah
    res_B = v[1]; 
    if (v[1] == 16) ds2450_p[2] = 0;
               else ds2450_p[2] = v[1];

    // Kanal C - Adr: 0ch
    res_C = v[2];
    if (v[2] == 16) ds2450_p[4] = 0;
               else ds2450_p[4] = v[2];

    // Kanal D - Adr: 0eh
    res_D = v[3];
    if (v[3] == 16) ds2450_p[6] = 0;
               else ds2450_p[6] = v[3];

    /*** Referenzspannungen ***/
    // Kanal A - Adr: 09h
    ds2450_p[1] = v[4];
    refspg_A = (v[4]*2.56) + 2.56;          // Kanal-Ref.spg. merken

    // Kanal B - Adr: 0bh
    refspg_B = (v[5]*2.56) + 2.56; 
    ds2450_p[3] = v[5];

    // Kanal C - Adr: 0dh
    refspg_C = (v[6]*2.56) + 2.56; 
    ds2450_p[5] = v[6];

    // Kanal D - Adr: 0fh
    refspg_D = (v[7]*2.56) + 2.56; 
    ds2450_p[7] = v[7];

    
    /********************************************************************/
    /*** Einschreiben der Init-Daten (= Inhalt ds2450_p[.]) in Page 1 ***/
    /********************************************************************/
    ds2450_write_page(1);


    /*******************************************************************/
    /*** Noch notwendigen Wert in Page 3 einschreiben unter Adr: 1ch ***/
    /*******************************************************************/

    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(25);

    // Adressierung des jeweils als aktiv ausgewählten Slaves
    ow_match();

    // Schreib-Befehl senden: "WRITE Memory" = 0x55
    ow_wr_byte(0x55);

    // Register-Start-Adresse übermitteln: TA1 und TA2
    ow_wr_byte(0x1c);           // TA1 = niederwertig = 1c
    ow_wr_byte(0x00);           // TA2 = höherwertig  = 00

    // 1. Datenbyte senden für Reg-Adr: 1ch
    ow_wr_byte(0x40);           // Datum = 40h

    // 16-Bit-CRC und Datum vom DS2450er zurück lesen
    crc1=ow_rd_byte();          // CRC-1
    crc2=ow_rd_byte();          // CRC-2

    // Kopie des gesendeten Datums vom DS2450er zurücklesen und vergleichen
    zw=ow_rd_byte();
    if (zw != 0x40)         // Fehler
    {
        ow_fem(26);         // Fehlermeldung ausgeben
    }

    // CRC-Überprüfung; entfällt zur Zeit !!

    // Fertig mit dem Senden dieses Bytes
    // Master sendet abschließenden Reset-Impuls
    ow_pres_check(29);
}

/*********************************************************************************************/

/*** Start der Wandlung und Auslesen für alle 4 Kanäle eines einzelnen DS2450ers am Bus ***/

void ds2450_convert_all(void)
{

    /****************************************/
    /*** Start Convert - Anweisung senden ***/
    /****************************************/
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(27);

    // Adressierung des jeweils als aktiv ausgewählten Slaves
    ow_match();

    // Start-Wandlung-Befehl senden: "Convert" = 0x3c
    ow_wr_byte(0x3c);

    // Senden der input-selct-Maske: alle Kanäle wandeln
    ow_wr_byte(0x0f);

    // Senden read out control byte: nicht unbedingt notwendig
    ow_wr_byte(0x55);

    // 16-Bit-CRC vom DS2450er zurück lesen
    crc1=ow_rd_byte();            // CRC-1
    crc2=ow_rd_byte();            // CRC-2

    // CRC-Überprüfung; entfällt zur Zeit !!

    // Warten bis Wandlung aller vier Kanäle fertig ist
    while(ow_rd_byte() != 0xff);

    // Abschließender Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(28);
 

    /*****************************************************************************/
    /*** Wandlungsergebnisse aus Page 0 auslesen u. in ds2450_p[.] abspeichern ***/
    /*****************************************************************************/
    ds2450_read_page(0);
}

/*********************************************************************************************/


/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

