/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS2408-Projekt - Digital I/O                           */
/*                        Programm unter Verwendung der 1-Wire-APIs:                      */
/*                           -  ow.h                                                      */
/*                           -  ow_slaves.h                                               */
/*                           -  ds2408.h                                                  */
/*                           -  ow_exbo.h                                                 */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   27.07.2010                                                      */
/*                                                                                        */
/*     Datei:             ds2408_demo_2.c                                                 */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow.h"                 // 1-Wire-Bus-API
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem
#include "ds2408.h"             // DS2408er-API
#include "ow_exbo.h"            // API speziell zum PT-1-Wire-ExBo


/*******************************************************************************************/
/*** Globale Variablen *********************************************************************/
/*******************************************************************************************/






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

    printf("Das grosse 1-Wire - DS2408er-Projekt - Digital I/O\n");
    printf("==================================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. Ausgabe eines Byte-Wertes\n\n");
    printf("  4. Blinken eines Port-Pins\n\n");
    printf("  5. Einlesen des gesamten Port-Zustandes\n\n");
    printf("  6. Zustand eines Port-Pins einlesen\n\n");
    printf("\n\n  9. ");
    printf("\n\n\nIhre Wahl:  ");
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

/*******************************************************************************************/
/*** Demo-Funktionen ***********************************************************************/
/*******************************************************************************************/

/*** Byte-Werte ausgeben ***/

void byte_aus(void)
{
    unsigned char s1[4];
    unsigned char wert;

    // Endloschleife
    while(1)
    {
        // Bildschirm löschen
        clear_screen();
    
        printf("Ausgabe eines Byte-Wertes\n");
        printf("==========================\n\n");
        printf("(Ruecksprung durch Eingabe von 255)\n\n\n");
        printf("  Byte-Wert in DEZ (0 .. 255):   ");
    
        // Byte-Wert einlesen
        inputse(s1,4);
    
        // Byte-Wert wandeln
        wert=atoi(s1);

        // Byte-Wert aussenden
        ds2408_wr_byte(wert);
    
        // Rücksprung
        if (wert == 0xff) break;

        // Löschen
        set_cursor(6,33); printf("  "); set_cursor(6,33);
    }
}

/*******************************************************************************************/

/*** Blinken eines Port-Pins ***/

void port_pin_blink(void)
{
    unsigned char nr;
    unsigned int zv;
    unsigned char s1[6];

    // Bildschirm löschen
    clear_screen();
    
    printf("Blinken eines Port-Pins\n");
    printf("=======================\n\n");
    printf("(Ende durch Tastendruck,  Ruecksprung durch Eingabe von 'e')\n\n\n");
    printf("  Port-Pin-Nr. (0 .. 7):   ");

    // Endloschleife
    while(1)
    {
        // Port-Pin-Nr. einlesen
        nr=getchar();
        if (nr == 'e') break;
        nr=nr-0x30; printf("%u",nr);
    
        printf("\n\n  Eingabe der Zeitverzoegerung in ms (1 ... 65535):  ");

        // Wert einlesen
        inputse(s1,6);

        // Byte-Wert wandeln
        zv=atoi(s1); if(zv == 0) zv=1;

        // Port-Pin blinken lassen
        while(1)
        {
            ds2408_set_port(nr,0);      // Port-Pin auf Low
            _wait_ms(zv);
            ds2408_set_port(nr,1);      // Port-Pin auf High
            _wait_ms(zv);

            // Aussprung
            if(seri_zeichen_da()) break;
        }

        // Löschen
        set_cursor(8,52); printf("       ");
        set_cursor(6,27); printf(" "); set_cursor(6,27);
    }
}

/*******************************************************************************************/

/*** Einlesen des gesamten Port-Zustandes ***/

void read_port(void)
{
    unsigned int zw;

    // Bildschirm löschen
    clear_screen();
    
    printf("Einlesen des gesamten Port-Zustandes\n");
    printf("====================================\n\n");
    printf("(Ende durch Tastendruck)\n\n\n");
    printf("  Port-Zustand in HEX  /  DEZ:");

    // Endloschleife
    while(1)
    {
        // Port-Zustand einlesen
        zw = ds2408_rd_zustand();

        // Ausgabe
        set_cursor(6,33);
        printf("%2x  /  %3u",zw,zw);

        // Aussprung
        if(seri_zeichen_da()) break;
    }
}

/*******************************************************************************************/

/*** Zustand eines Port-Pins einlesen ***/

void read_port_pin(void)
{
    unsigned char nr,zw;

    // Bildschirm löschen
    clear_screen();
    
    printf("Zustand eines Port-Pins einlesen\n");
    printf("================================\n\n");
    printf("(Ende durch Tastendruck,  Ruecksprung durch Eingabe von 'e')\n\n\n");
    printf("  Port-Pin-Nr. (0 .. 7):\n\n");
    printf("  Zustand:");

    // Endloschleife
    while(1)
    {
        // Port-Pin-Nr. einlesen
        set_cursor(6,27);
        nr=getchar();
        if (nr == 'e') break;
        nr=nr-0x30; printf("%u",nr);
    
        // Zustand des Port-Pins einlesen und darstellen
        while(1)
        {
            // Zustand einlesen
            zw=ds2408_get_port(nr);

            // Darstellen
            set_cursor(8,12); printf("%1u",zw);

            // Aussprung
            if(seri_zeichen_da()) break;
        }

        // Löschen
        set_cursor(8,12); printf(" ");
        set_cursor(6,27); printf(" "); set_cursor(6,27);
    }
}

/*******************************************************************************************/


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

    // Wartezeit für 1-Wire-Slaves
    _wait_ms(100);

    // Den aktiven 1-Wire-Slave für die nachfolgende Kommunikation festlegen
    ow_sl_aktiv = 14;       // Slave Nr 14: DS2450er - 1 - Digital I/O

    // Initialisierung des DS2408ers: Übergabe des entspr. Bytes fürs Control-Reg.
    ds2408_init(0x04);      // RSTZ-Pin als STRB\-Ausgang

    // Flanken-Wechsel-FFs zurücksetzen
    ds2408_fla_cl();

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

            case '3':   byte_aus();         // Byte-Wert ausgeben
                        break;

            case '4':   port_pin_blink();   // Blinken eines Port-Pins
                        break;

            case '5':   read_port();        // Einlesen des gesamten Port-Zustande
                        break;

            case '6':   read_port_pin();    // Zustand eines Port-Pins einlesen
                        break;            

            case '9':   ;
                        break;

        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/