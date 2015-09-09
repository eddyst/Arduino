/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS2408 - LC-Display - Projekt                          */
/*                        Programm unter Verwendung der 1-Wire-APIs:                      */
/*                           -  ow.h                                                      */
/*                           -  ow_slaves.h                                               */
/*                           -  ds2408.h                                                  */
/*                           -  ds2408_lcd.h                                              */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   16.10.2010                                                      */
/*                                                                                        */
/*     Datei:             ds2408_lcd_2.c                                                  */
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
#include "ds2408_lcd.h"         // LCD-API für DS2408er
#include "ow_exbo.h"            // API speziell zum PT-1-Wire-ExBo


/*******************************************************************************************/
/*** Globale Variablen *********************************************************************/
/*******************************************************************************************/

// Fest-Texte für die LCD-Ausgabe
unsigned char txt_0[21]=" PT-Mikrocontroller ";
unsigned char txt_1[21]="********************";
unsigned char txt_2[21]="  Full Power with:  ";
unsigned char txt_3[21]="   PT-1Wire-ExBo    ";
unsigned char txt_4[21]="Die ersten Zeichen:";
unsigned char txt_5[21]="Deutsche Sonderzei.:";


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

    printf("Das grosse 1-Wire - DS2408er - LC-Display - Projekt\n");
    printf("===================================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. LC-Display - Demo-Betrieb\n\n");
    printf("  4. Taster - Demo-Betrieb\n\n");
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

/*** DS2408er-LCD-Demo-Programm ***/

void ds2408_lcd_demo(void)
{
    unsigned char i;
    unsigned int zae=0;

    // Bilschirm löschen
    clear_screen();

    printf("LCD-Demo-Betrieb:\n");
    printf("=================\n\n");
    printf("Laeuft  --  Abbruch mit Reset ... ");

    // Endlosschleife
    while(1)
    {
        // LCD löschen
        ds2408_lcd_clear();

        // LCD ein, mit blinkendem Cursor
        ds2408_lcd_on(2);

        _wait_ms(1000);

        // ´LCD´ darstellen
        ds2408_lcd_wr(0x4c,1);
        ds2408_lcd_wr(0x43,1);
        ds2408_lcd_wr(0x44,1);

        _wait_ms(1000);

        // LCD ausschalten
        ds2408_lcd_off();

        _wait_ms(1000);

        // LCD ein, mit Cursor
        ds2408_lcd_on(1);

        _wait_ms(1000);

        // LCD löschen
        ds2408_lcd_clear();

        _wait_ms(1000);

        // Deutsche Sonderzeichen ausgeben
        ds2408_lcd_dis_txt(0,0,txt_5);

        // Cursor setzen
        ds2408_lcd_csr_set(2,3);

        ds2408_lcd_dis_zei('{');
        ds2408_lcd_dis_zei('|');
        ds2408_lcd_dis_zei('}');
        ds2408_lcd_dis_zei('~');

        _wait_ms(1000);

        // Zahlen am Zeilenende ausgeben
        ds2408_lcd_dis_csr_zei(0,19,'0');
        ds2408_lcd_dis_csr_zei(1,19,'1');
        ds2408_lcd_dis_csr_zei(2,19,'2');
        ds2408_lcd_dis_csr_zei(3,19,'3');

        _wait_ms(1500);

        // LCD löschen
        ds2408_lcd_clear();

        // Cursor aus
        ds2408_lcd_on(0);

        // Festtexte ausgeben
        ds2408_lcd_dis_txt(0,0,txt_0);
        ds2408_lcd_dis_txt(1,0,txt_1);
        ds2408_lcd_dis_txt(2,0,txt_2);
        ds2408_lcd_dis_txt(3,0,txt_3);

        _wait_ms(2000);

        // LCD blinken
        for (i=0; i<5; i++)
        {
            ds2408_lcd_off();       // Aus
            _wait_ms(750);
            ds2408_lcd_on(0);       // Ein
            _wait_ms(750);
        }

        zae=zae+1;
        printf("\n%5u: Fertig !",zae);
    }
}

/****************************************************************************************/

/*** DS2408er-LCD- Tasten - Demo-Programm ***/

void ds2408_lcd_ta_demo(void)
{
    unsigned char zw;

    // Bilschirm löschen
    clear_screen();

    printf("LCD- Taster -Demo-Betrieb:\n");
    printf("==========================\n\n");
    printf("Laeuft  --  Abbruch mit Reset ... \n\n");
    printf("Bitte eine Taste druecken (T4 - T6):   ");

    // Endlosschleife
    while(1)
    {
        zw = ow_exbo_lcd_ta_in();   // Tastendrücke einlesen
        set_cursor(10,10);
        printf("Gedrueckte Taste: %2x",zw);
    }
}

/****************************************************************************************/



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

    // Wartezeit für die 1-Wire-Slaves
    _wait_ms(300);

    // Den aktiven 1-Wire-Slave für die nachfolgende Kommunikation festlegen
    ow_sl_aktiv = 15;       // Slave Nr 15: DS2450er - 2 - LCD


/*

// Kritischer Zustand

    // Master Reset / Slave Presensce
    ow_pres_check(50);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Rücksprung
    ow_wr_byte(0x3c);

    // Master Reset / Slave Presensce
    ow_pres_check(50);


// Ende

*/






    // Initialisierung des DS2408ers: Übergabe des entpsr. Bytes fürs Control-Reg.
    ds2408_init(0x04);      // 04h: RTSZ-Pin als STRB\
                            // 00h: RTSZ-Pin als RST\

    // Flanken-Wechsel-FFs zurücksetzen
//    ds2408_fla_cl();


    // LCD initialsieren
    ds2408_lcd_init();


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

            case '3':   ds2408_lcd_demo();      // LCD-Demo
                        break;

            case '4':   ds2408_lcd_ta_demo();   // Tastendrücke erfassen
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