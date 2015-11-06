/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS2540-Projekt                                         */
/*                        Programm unter Verwendung der 1-Wire-APIs:                      */
/*                           -  ow.h                                                      */
/*                           -  ow_slaves.h                                               */
/*                           -  ds2450.h                                                  */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   12.05.2010                                                      */
/*                                                                                        */
/*     Datei:             ds2540_demo_2.c                                                 */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow.h"                 // 1-Wire-Bus-API
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem

#include "ds2450.h"             // DS2450er-API



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
    ow_sl_aktiv = 10;

    // Initialisierung des DS2450ers: Übergabe des entpsr. Init-Vektors
    ds2450_init(ds2450_init_v0);


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