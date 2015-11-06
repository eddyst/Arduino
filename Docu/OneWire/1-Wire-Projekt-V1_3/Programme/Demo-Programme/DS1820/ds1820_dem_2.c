/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS1820-Projekt                                         */
/*                        Programm MIT Verwendung der ´ow´- und ´ds1820´-APIs             */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   03.09.2010                                                      */
/*                                                                                        */
/*     Datei:             ds1820_dem_2.c                                                  */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow_slaves.h"          // Adressen aller im System eingesetzten 1-Wire-Slaves

#include "ow.h"                 // 1-Wire-Bus-Grundfunktionen
#include "ds1820.h"             // DS1820-spezifische Funktionen


/*******************************************************************************************/
/*** Globale Variablen *********************************************************************/
/*******************************************************************************************/





/*******************************************************************************************/
/*** Applikations-spezifische Funktionen ***************************************************/
/*******************************************************************************************/

/*** Haupt-Menü-Bild ***/
void haupt_menue(void)
{
    clear_screen();

    printf("Das grosse 1-Wire - DS18S20er-Projekt\n");
    printf("=====================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. Demo - Single Chip: Temperaturmessung und Lesen des Scratch Pads\n\n");
    printf("  4. Single Chip: 2 Byte (TH,TL) in Scratch Pad schreiben\n\n");
    printf("  5. Demo - Bus-Betrieb mit max. 5 Sensoren\n\n");
    printf("\n\n  9. Test zur uC-spezifischen Delay-Festlegung");
    printf("\n\n\n\nIhre Wahl:  ");
}

/*******************************************************************************************/

/*** Test zur uC-spezifischen Bestimmung des exakten OW-Delay-Wertes ***/

void delay_test(void)
{
    unsigned char s1[4];
    unsigned char delay,c1,i;

    while(1)
    {
        clear_screen();
        printf("Test zur uC-spezifischen Delay-Festlegung\n");
        printf("=========================================\n\n");
        printf("VOR dem Test den DS18S20 abklemmen !\n\n");
        printf("Es gibt nun zwei Moeglichkeiten, die notwendigen Zeitverzoegerungen\n");
        printf("zu erzeugen:\n\n");
        printf("  1. Mit einer reinen for-Schleife direkt im Programm-Code.\n");
        printf("  2. Durch Aufruf einer Funktion, die eine for-Schleife enthaelt.\n\n");
        printf("Die Methode 1 wird zur Erzeugung sehr kleiner Zeitverzoegerungen (< 20 us),\n");
        printf("die Methode 2 fuer groessere Verzoegerungen verwendet.\n\n");
        printf("Auswahl, was getestet werden soll (1, 2 oder 'e' (Ende)):  ");

        c1 = getchar();

        if ((c1 != '1') && (c1 != '2')) break;           // Rücksprung

        if (c1 == '1')              // Test für Methode 1
        {
            while(1)
            {
                clear_screen();
                printf("Test Methode 1: Zeitverzoegerung d. reine for-Schleife direkt im Progamm-Code\n");
                printf("=============================================================================\n");
                printf("(Ruecksprung durch Reset ... )\n\n");

                printf("\n\nTest laeuft ... ");
    
                while(1)
                {
                    DQ = 0;
                    for(i=0; i<8; i++);         // for-Schleife zur Zeitverzögerung:
                                                // Wert muss direkt eingetragen werden,
                                                // keine Variable verwenden: würde dadurch
                                                // langsamer !!!!
                    DQ = 1;
                    _wait_ms(10);
                }
            }
        }

        else                        // Test für Methode 2
        {
            while(1)
            {
                clear_screen();
                printf("Test Methode 2: Zeitverzoegerung durch Funktionsaufruf\n");
                printf("======================================================\n");
                printf("(Ruecksprung durch Eingabe von 255)\n\n");
                printf("Bitte Delay-Wert (Funktions-Uebergabe-Wert) eingeben (0 ... 254): ");
    
                inputse(s1,4);
                delay = atoi(s1);
    
                if (delay == 255) break;            // Rücksprung
    
                printf("\n\nTest laeuft, weiter mit Taste ... ");
    
                while(1)
                {
                    DQ = 0;
                    ow_delay(delay);
                    DQ = 1;
                    if (seri_zeichen_da()) break;
                    _wait_ms(10);
                }
            }
        }
    }
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
    unsigned char i,c1;

    // Endlosschleife
    while(1)
    {
        clear_screen();
        printf("Single Chip: Lesen des 64 Bit ROM-Identifiers\n");
        printf("=============================================\n");
        printf("Abfrage durch Druck auf Taste,  Beenden durch Eingabe von 'e':  ");

        // ds1820angsbuffer löschen    
        for(i=0; i<8; i++) ds1820[i]=0;
    
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

/*********************************************************************************************/

/*** DS1820: Temperaturmessung und Lesen des Scratch Pads ***/

void ds1820_get_temp(void)
{
    unsigned char i;
    float ftemp;
    unsigned int zv;
    unsigned char s1[6];

    clear_screen();
    printf("Single Chip: Temperaturmessung und Lesen des Scratch Pads\n");
    printf("=========================================================\n\n");
    printf("Bitte Abfrage-Intervall in ms eingeben (100 ... 65.000 ms):  ");

    // Zeit einlesen
    inputse(s1,6);
    zv = atoi(s1);

    // Bildschirm-Maske
    set_cursor(6,2); printf("Inhalt des Scratch Pads:");
    set_cursor(8,4); printf("Byte:     8     7     6     5     4     3     2     1     0");
    set_cursor(10,4); printf("(HEX)");

    set_cursor(14,2); printf("Aktuelle Temperatur:            C");
    set_cursor(18,0); printf("Abbruch durch Tastendruck ...");

    // ds1820 Ausgangsbuffer löschen    
    for(i=0; i<9; i++) ds1820[i]=0;

    // Aktiven 1-Wire-Slave am Bus festlegen
    ow_sl_aktiv=255;        // 255 = nur ein Slave am Bus !

    // Endlosschleife
    while (1)
    {
        // Scratch Pad und Temp. des DS1820 auslesen
        ftemp=ds1820_rd_temp();

        // Scratch Pad-Inhalt ausgeben
        set_cursor(10,14);
        printf("%02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x",ds1820[8],ds1820[7],ds1820[6],ds1820[5],ds1820[4],ds1820[3],ds1820[2],ds1820[1],ds1820[0]);

        // Ausgabe der Temp.
        set_cursor(14,26); printf("%5.1f",ftemp);

        // Abfrage auf Abbruch
        if (seri_zeichen_da()) break;

        // Warten bis zur nächsten Messung
        _wait_ms(zv);
    }
}

/*******************************************************************************************/

/*** DS1820 - Demo: Beschreiben der 2 Bytes im Scratch Pad ***/

void ds1820_scr_pa_wr_demo(void)
{
    unsigned char s1[4];
    unsigned char TH,TL,c1;

    clear_screen();
    printf("Single Chip: 2 Byte (TH,TL) in Scratch Pad schreiben\n");
    printf("====================================================\n\n");
    printf("Bitte beide Werte (0 ... 255) nachfolgend einzeln eingeben:\n\n\n");

    printf("  Wert fuer Byte 2 im Scratch Pad (TH-Register):  ");
    inputse(s1,4);
    TH = atoi(s1);

    printf("\n\n  Wert fuer Byte 3 im Scratch Pad (TL-Register):  ");
    inputse(s1,4);
    TL = atoi(s1);

    printf("\n\n\nUebertragung ins Scratch Pad durch Tastendruck ... ");
    c1 = getchar();

    // Aktiven 1-Wire-Slave am Bus festlegen
    ow_sl_aktiv=255;        // 255 = nur ein Slave am Bus !

    // Übertragung der beiden Werte
    ds1820_wr_t_reg(TH,TL);

    printf("\n\n\nUebertragung o.k.,  Taste druecken ... ");
    c1 = getchar();
}

/*******************************************************************************************/

/*** Bemo-Betrieb mit bis zu 5 DS18S20er-Slaves ***/

void bus_demo(void)
{
    unsigned char i,merker;
    float ftemp;

    clear_screen();
    printf("1-Wire-Bus-Demo fuer max. 5 DS18S20er-Slaves\n");
    printf("============================================\n\n");
    printf("Demo laeuft ...   (Abbruch durch Tastendruck)\n\n\n");

    printf("   Slave-Nr.     Temperatur         Scratch-Pad (Byte 8 .... Byte 0)");

    // Endlosschleife
    while(1)
    {
        merker = 0;

        // Slaves abfragen: Slave-Nummern 0 - 4
        for (i=0; i<5; i++)
        {
            // Aktiven 1-Wire-Slave am Bus festlegen
            ow_sl_aktiv=i;        // i = Stellung in der Adress-Matrix

            // Scratch Pad und Temperatur des DS1820 auslesen
            ftemp=ds1820_rd_temp();

            // Scratch Pad-Inhalt ausgeben
            set_cursor(2*i+8,36);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x",ds1820[8],ds1820[7],ds1820[6],ds1820[5],ds1820[4],ds1820[3],ds1820[2],ds1820[1],ds1820[0]);

            // Prüfen, ob DS18S20er sich ordnungsgemäß zurückgemeldet hat
            if (ftemp == 555)       // Falsche Summe über Scratch-Pad-Bytes
            {
                set_cursor(2*i+8,6); printf("%u          Keine kor. Antw.",i);
                merker = 2;
            }
            else merker = 0;
            
            // Ausgabe nur, wenn Slave-Ant. korrekt war
            if (merker == 0)
            {
                // Ausgabe: Slave-Nr. und Temp.
                set_cursor(2*i+8,6); printf("%u            %5.1f         ",i,ftemp);
            }

            // Abfrage auf Abbruch
            if (seri_zeichen_da()) 
            {
                merker = 1;
                break;
            }

            // Warten bis zur nächsten Messung
            _wait_ms(50);           

        }
        if (merker == 1) break;          // Aussprung
    }
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

    // Aktiven 1-Wire-Slave am Bus festlegen
    ow_sl_aktiv=255;        // 255 = nur ein Slave am Bus !

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

            case '3':   ds1820_get_temp();
                        break;

            case '4':   ds1820_scr_pa_wr_demo();
                        break;

            case '5':   bus_demo();
                        break;

            case '9':   delay_test();
                        break;

        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/