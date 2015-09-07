/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS2540-Projekt                                         */
/*                        Programm unter Verwendung der 1-Wire-APIs:                      */
/*                           -  ow.h                                                      */
/*                           -  ow_slaves.h                                               */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   07.06.2010                                                      */
/*                                                                                        */
/*     Datei:             ds2540_demo_1.c                                                 */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow.h"                 // 1-Wire-Bus-API
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem


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


/*******************************************************************************************/
/*** Selbstgeschriebene Funktionen *********************************************************/
/*******************************************************************************************/

/*******************************************************************************************/
/*** Applikations-spezifische Funktionen ***************************************************/
/*******************************************************************************************/

/*** Haupt-Menü-Bild ***/
void haupt_menue(void)
{
    clear_screen();

    printf("Das grosse 1-Wire - DS2450er-Projekt\n");
    printf("=====================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. Single Chip - Demo-Betrieb\n\n");
    printf("  4. \n\n");
    printf("  5. \n\n");
    printf("\n\n  9. ");
    printf("\n\n\n\nIhre Wahl:  ");
}

/*******************************************************************************************/

/*** Demo zum 1-Wire-Reset ***/

void ow_reset_demo(void)
{
    unsigned char c1;

    clear_screen();
    printf("1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n");
    printf("======================================================\n");
    printf("(Beenden durch Eingabe von 'e')");
    printf("\n\nStart des Tests:  Taste druecken ... ");

    while(1)
    {
        set_cursor(7,2); printf("                                      ");
        set_cursor(7,2);
        c1=getchar();
        if (c1 == 'e') break;        // Rücksprung

        // 1-Wire Reset durchführen
        if (ow_reset()) printf("  Test NEGATIV:  Keiner da !\n");
        else printf("  Test POSITIV:  Mindestens EINER da !\n");

        printf("\n\nWeiter - Taste druecken ... ");
        c1 = getchar();
    }
}

/*********************************************************************************************/

/** Lesen des ROM-Identifiers bei einem einzelnen 1-Wire-Slave am Bus ***/

void ow_rd_rom_id(void)
{
    unsigned char c1;

    // Endlosschleife
    while(1)
    {
        clear_screen();
        printf("Single Chip: Lesen des 64 Bit ROM-Identifiers\n");
        printf("=============================================\n");
        printf("Abfrage durch Druck auf Taste,  Beenden durch Eingabe von 'e':  ");

        // Warten auf Tastendruck
        c1=getchar();
        if (c1 == 'e') break;                   // Rücksprung

        // Einlesen des ROM-Identifieres
        ow_rd_rom();

        // Ausgabe
        printf("\n\n\n  8-Bit CRC      48-Bit Serial Number      8-Bit Familiy Code\n\n");
        printf("     %02x           %02x %02x %02x %02x %02x %02x               %02x             (HEX)\n",rom_c[7],rom_c[6],rom_c[5],rom_c[4],rom_c[3],rom_c[2],rom_c[1],rom_c[0]);
    
        printf("\n\n\n\nFertig, Taste druecken ... "); c1=getchar();
    }
}

/*******************************************************************************************/

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
    ow_wr_byte(0x00);

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

/*** Demo-Betrieb für einen einzelnen DS2450er am Bus ***/

void ds2450_single_demo(void)
{
    unsigned int wert;
    float spg;

    // Bildschirm-Maske
    clear_screen();
    printf("Demo-Betrieb fuer einen DS2450er am Bus\n");
    printf("=======================================\n\n\n");
    printf("  Kanal    Aufl.   Uref / V    MSByte   LSByte   Dig.Wert(dez)    Analog / mV\n");
    printf("\n   A\n");
    printf("\n   B\n");
    printf("\n   C\n");
    printf("\n   D");

    // Endlosschleife
    while(1)
    {
        // Abfrage des DS2450ers: Werte unter ds245_p[.] abgespeichert
        ds2450_convert_all();


        /*********************************/
        /*** Darstellung der Messwerte ***/
        /*********************************/

        // Für Kanal A
        wert = ((ds2450_p[1] * 256) + ds2450_p[0]);
        wert = wert >> (16 - res_A);

        spg = wert * (refspg_A / pot2[res_A]);
        spg = spg * 1000;
    
        set_cursor(6,12);
        printf("%2u      %4.2f        %3u      %3u       %5u           %8.3f",res_A,refspg_A,ds2450_p[1],ds2450_p[0],wert,spg);
        
        // Für Kanal B
        wert = ((ds2450_p[3] * 256) + ds2450_p[2]);
        wert = wert >> (16 - res_B);

        spg = wert * (refspg_B / pot2[res_B]);
        spg = spg * 1000;
    
        set_cursor(8,12);
        printf("%2u      %4.2f        %3u      %3u       %5u           %8.3f",res_B,refspg_B,ds2450_p[3],ds2450_p[2],wert,spg);
        
        // Für Kanal C
        wert = ((ds2450_p[5] * 256) + ds2450_p[4]);
        wert = wert >> (16 - res_C);
    
        spg = wert * (refspg_C / pot2[res_C]);
        spg = spg * 1000;
    
        set_cursor(10,12);
        printf("%2u      %4.2f        %3u      %3u       %5u           %8.3f",res_C,refspg_C,ds2450_p[5],ds2450_p[4],wert,spg);
    
        // Für Kanal D
        wert = ((ds2450_p[7] * 256) + ds2450_p[6]);
        wert = wert >> (16 - res_D);
    
        spg = wert * (refspg_D / pot2[res_D]);
        spg = spg * 1000;
    
        set_cursor(12,12);
        printf("%2u      %4.2f        %3u      %3u       %5u           %8.3f",res_D,refspg_D,ds2450_p[7],ds2450_p[6],wert,spg);

        // Wartezeit zwsichen den Messungen
        _wait_ms(400);

    }       // Ende Endloschleife
}

/********************************************************************************************/


/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/


/*******************************************************************************************/
/*** Start des Hauptprogramms **************************************************************/
/*******************************************************************************************/

void main (void)     // Start des Hauptprogramms
{
    unsigned char c1;

    // Initialisierung der ser. Schnittstelle des CC03ers
    seri_init();

    // Bildschirm löschen
    clear_screen();


    // Den aktiven 1-Wire-Slave für die nachfolgende Kommunikaiton festlegen
    ow_sl_aktiv = 10;       // Slave Nr 10: DS2450er - 0

    // Initialisierung des DS2450ers: Übergabe des entpsr. Init-Vektors
    ds2450_init(ds2450_init_v2);


    // Endlosschleife
    while(1)
    {
        // Hauptmenü aufrufen
        haupt_menue();

        // Eingabe und Auswertung
        c1 = getchar();

        switch (c1)
        {
            case '1':   ow_reset_demo();
                        break;

            case '2':   ow_rd_rom_id();
                        break;

            case '3':   ds2450_single_demo();
                        break;

            case '4':   ;
                        break;

            case '5':   ;
                        break;

            case '9':   ;
                        break;

        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/