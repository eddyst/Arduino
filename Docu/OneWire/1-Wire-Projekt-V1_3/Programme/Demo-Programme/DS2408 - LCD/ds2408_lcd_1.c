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
/*     Datei:             ds2408_lcd_1.c                                                  */
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

/*******************************************************************************************/
/*** DS2408er-LC-Display-spezifische Funktionen ********************************************/
/*******************************************************************************************/

/*** Ein Byte, gesplittet in zwei Niibles, in ein LCD-Register einschreiben ***/

void ds2408_lcd_wr(unsigned char dat, unsigned char reg)
{
    unsigned char zw1,zw2;

    // Oberes Halbbyte selektieren
    zw1 = dat >> 4;

    // Unteres Halbbyte selektieren
    zw2 = dat & 0x0f;

    // LCD-Ziel-Register-Info hinzufügen
    if (reg == 1)           // Ziel = LCD-Daten-Reg -> RS = 1
    {
        zw1 = zw1 + 0x10;
        zw2 = zw2 + 0x10;
    }

    // PT-ExBo-spezifisch für Abfrage der Taster T4 - T6: Ports PIO5 - PIO7 auf 1
    zw1 = zw1 + 0xe0;
    zw2 = zw2 + 0xe0;

    // Bytes nun aussenden
    ds2408_wr_byte(zw1);
    ds2408_wr_byte(zw2);
}

/*****************************************************************************************/

/*** LCD initialisieren im 4 (!)-Bit-Modus ***/

void ds2408_lcd_init(void)
{
    // 15 ms warten
    _wait_ms(15);

    // Ausgabe: 03h
    ds2408_wr_byte(0x03);

    // 4,1 ms warten
    _wait_ms(5);

    // Ausgabe: 03h
    ds2408_wr_byte(0x03);

    // 100 us warten
    _wait_ms(1);

    // Ausgabe: 03h
    ds2408_wr_byte(0x03);

    // Function Set: 4 Bit Bus
    ds2408_wr_byte(0x02);

    // Function Set: 1/16; 5*7 Matrix: 0010 1000
    ds2408_lcd_wr(0x28,0);

    // Display OFF: Display off, Cursor off, Blink off: 0000 1000
    ds2408_lcd_wr(0x08,0);

    // Dislay clear: 0000 0001
    ds2408_lcd_wr(0x01,0);

    // Entry Mode Set: increment, no disply shift: 0000 0110
    ds2408_lcd_wr(0x06,0);
}

/*****************************************************************************************/

/*** LCD löschen ***/

void ds2408_lcd_clear(void)
{
    ds2408_lcd_wr(0x01,0);        // Löschbefehl senden
}

/*****************************************************************************************/

/*** LCD ausschalten, Daten bleiben jedoch erhalten ***/

void ds2408_lcd_off(void) 
{
    ds2408_lcd_wr(0x08,0);        // Befehl senden
}

/*****************************************************************************************/

/*** LCD einschalten, mit/ohne Cursor ***/

void ds2408_lcd_on(unsigned char n)
{
    switch (n)              // Display ON/OFF Controll
    {
        case 0 : ds2408_lcd_wr(0x0c,0);     // ohne Cursor
                 break;
        case 1 : ds2408_lcd_wr(0x0e,0);     // mit Cursor
                 break;
        case 2 : ds2408_lcd_wr(0x0f,0);     // mit blinkendem Cursor
                 break; 
    }
}

/*****************************************************************************************/

/*** Cursor auf LCD positionieren: Zeile,Spalte ***/

void ds2408_lcd_csr_set(unsigned char z, unsigned char s)
{
    unsigned char csr_pos;

    switch (z)                      // Reale Cursor-Adresse ermitteln
    {
        case 0 : csr_pos=s;         // Zeile 0
                 break;
        case 1 : csr_pos=s+0x40;    // Zeile 1
                 break;
        case 2 : csr_pos=s+0x14;    // Zeile 2
                 break;
        case 3 : csr_pos=s+0x54;    // Zeile 3
                 break;
    }

    ds2408_lcd_wr((csr_pos | 0x80),0);      // Cursor-Adresse übermitteln: DD-RAM-Address Set
  }

/*****************************************************************************************/

/*** Zeichen an der aktuellen Cursor-Position ausgeben ***/

void ds2408_lcd_dis_zei(unsigned char zeichen)
{
    switch (zeichen)                // Deutsche Sonderzeichen anpassen
    {
        case '{' : zeichen=225; 
                   break;
        case '|' : zeichen=239; 
                   break;
        case '}' : zeichen=245; 
                   break;
        case '~' : zeichen=226; 
                   break;
    }

    ds2408_lcd_wr(zeichen,1);               // Zeichen ausgeben
}

/*****************************************************************************************/

/*** Zeichen an übergebener Cursor-Position ausgeben ***/

void ds2408_lcd_dis_csr_zei(unsigned char z, unsigned char s, unsigned char c)
{
    ds2408_lcd_csr_set(z,s);        // Cursor auf (z, s) positinieren
    ds2408_lcd_dis_zei(c);          // Zeichen ausgeben
}

/*****************************************************************************************/

/*** Text (String) ab Cursor-Position darstellen ***/

void ds2408_lcd_dis_txt(unsigned char z, unsigned char s, unsigned char *a)
{
    unsigned char i;

    ds2408_lcd_csr_set(z,s);        // Cursor setzen

    i=0;                            // Textstring auf Display ausgeben
    while(a[i]!=0)
    {
        ds2408_lcd_dis_zei(a[i]);   // Zeichen ausgeben
        i++;                        // nächstes Zeichen
    }
}

/*****************************************************************************************/

/*** PT-1-Wire-ExBo-Funktion: Scannen der LCD-Baugruppen-Tasten auf dem ExBo  ***/

unsigned char ow_exbo_lcd_ta_in(void)
{
    unsigned char zw;

    // Gesamten PIO-Port einlesen
    zw=ds2408_rd_zustand();

    // Tasten-Bits selektieren
    zw= zw & 0xe0;

    // Tasten-Bits auswerten und Rückgabe
    switch(zw)
    {
        case 0xe0:  return(0xff); break;        // Keine Taste gedrückt
        case 0xc0:  return(4); break;           // Taste 4
        case 0xa0:  return(5); break;           // Taste 5
        case 0x60:  return(6); break;           // Taste 6
    }

    return(0xff);       // Dummy-Return, wg. Compiler-Warning
}

/*****************************************************************************************/

/*** DS2408er-LCD-Demo-Programm ***/

void ds2408_lcd_demo(void)
{
    unsigned char i;

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

        printf("\nFertig !");
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
    _wait_ms(100);

    // Den aktiven 1-Wire-Slave für die nachfolgende Kommunikation festlegen
    ow_sl_aktiv = 15;       // Slave Nr 15: DS2450er - 2 - LCD

    // Initialisierung des DS2408ers: Übergabe des entpsr. Bytes fürs Control-Reg.
    ds2408_init(0x04);      // 04h: RTSZ-Pin als STRB\
                            // 00h: RTSZ-Pin als RST\

    // Flanken-Wechsel-FFs zurücksetzen
    ds2408_fla_cl();


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