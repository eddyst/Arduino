/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - Projekt:   Grosse Gesamt-Demo                          */
/*                        Programm unter Verwendung aller 1-Wire-APIs:                    */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   17.10.2010                                                      */
/*                                                                                        */
/*     Datei:             ow_ges_demo.c                                                   */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow.h"                 // 1-Wire-Bus-API
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem

#include "ds1820.h"             // DS1820er-API

#include "ds2450.h"             // DS2450er-API

#include "ds2408.h"             // DS2408er-API
#include "ds2408_lcd.h"         // LCD-API für DS2408er
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

/*** Terminal-Maske ***/
void term_maske(void)
{
    // Bildschirm löschen
    clear_screen();

    printf("Die grosse 1-Wire-Gesamt-Demo\n");
    printf("=============================\n\n");

    set_cursor(4,2); printf("Erfasste Temperaturen:");
    set_cursor(5,2); printf("======================");
    set_cursor(7,4); printf("Temperatur-INNEN:         C");
    set_cursor(7,40); printf("Temperatur-AUSSEN:         C");

    set_cursor(11,2); printf("Erfasste Spannungen:");
    set_cursor(12,2); printf("====================");
    set_cursor(14,4); printf("Kanal-A:          mV");
    set_cursor(14,40); printf("Kanal-B:          mV");
    set_cursor(16,4); printf("Kanal-C:          mV");
    set_cursor(16,40); printf("Kanal-D:          mV");
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
    unsigned char term;
    float temp1,temp2;
    float spgA,spgB,spgC,spgD;
    unsigned char s1[6];

    // Initialisierung der ser. Schnittstelle des CC03ers
    seri_init();

    // Bildschirm löschen
    clear_screen();

    printf("Die grosse 1-Wire-Gesamt-Demo\n");
    printf("=============================\n\n");
    printf("Ausgabe der Daten auch auf dem Terminal (j/n): ");
    term=getchar(); 

    // Auswertung
    if (term == 'j')
    {
        term_maske();
    }
    else
    {
        clear_screen();
        printf("Die grosse 1-Wire-Gesamt-Demo laeuft auf dem LC-Display ... ");
    }

    /*************************/
    /*** Initialisierungen ***/
    /*************************/

   // Aktiver Slave
    ow_sl_aktiv = 10;       // Slave Nr 10: DS2450er - 0 - A/D-W

    // DS2450 initialisieren: Alle - 8 Bit Auflösung, 5,12 V Refspg
    ds2450_init(ds2450_init_v0);


    // Aktiver Slave
    ow_sl_aktiv = 15;       // Slave Nr 15: DS2408er - 2 - LCD

    // Initialisierung des DS2408ers: Übergabe des entpsr. Bytes fürs Control-Reg.
    ds2408_init(0x04);      // 04h: RTSZ-Pin als STRB\
                            // 00h: RTSZ-Pin als RST\

    // LCD initialsieren
    ds2408_lcd_init();

    /*** Maske für LCD ausgeben ***/
    ds2408_lcd_clear();         // LCD löschen
    ds2408_lcd_on(0);           // LCD ein, ohne Cursor
    ds2408_lcd_dis_txt(0,0,"I:      C  A:      C");
    ds2408_lcd_dis_txt(1,0,"A:      V  B:      V");
    ds2408_lcd_dis_txt(2,0,"C:      V  D:      V");


    // Endlosschleife: messen und darstellen
    while(1)
    {
        /*** Einlesen der beiden Temperaturen ***/
        ow_sl_aktiv = 1;                // Fest eingebauter DS1820er
        temp1 = ds1820_rd_temp();       // Temp. lesen

        ow_sl_aktiv = 3;                // DS1820er in Fassung
        temp2 = ds1820_rd_temp();       // Temp. lesen


        /*** Einlesen der 4 Spannungen ***/
        ow_sl_aktiv = 10;               // DS2450er auf de PT-1-Wire-Board
        ds2450_convert_all();           // Alle Kanaäle wandeln

        // Spannungen berechnen, in mV
        spgA = ds2450_p[1] * 20;
        spgB = ds2450_p[3] * 20;
        spgC = ds2450_p[5] * 20;
        spgD = ds2450_p[7] * 20;

        /*** Ausgabe der Werte auf dem LC-Display ***/
        ow_sl_aktiv = 15;       // Slave Nr 15: DS2408er - 2 - LCD
        sprintf(s1,"%5.1f",temp1); ds2408_lcd_dis_txt(0,2,s1);
        sprintf(s1,"%5.1f",temp2); ds2408_lcd_dis_txt(0,13,s1);

        sprintf(s1,"%5.3f",spgA/1000); ds2408_lcd_dis_txt(1,2,s1);
        sprintf(s1,"%5.3f",spgB/1000); ds2408_lcd_dis_txt(1,13,s1);
        sprintf(s1,"%5.3f",spgC/1000); ds2408_lcd_dis_txt(2,2,s1);
        sprintf(s1,"%5.3f",spgD/1000); ds2408_lcd_dis_txt(2,13,s1);

        /*** Ausgabe der Werte auf dem Terminal-Monitor ***/
        if (term == 'j')
        {
            // Temperaturen
            set_cursor(7,24); printf("%5.1f",temp1);
            set_cursor(7,60); printf("%5.1f",temp2);

            set_cursor(14,15); printf("%6.1f",spgA);
            set_cursor(14,51); printf("%6.1f",spgB);           
            set_cursor(16,15); printf("%6.1f",spgC);   
            set_cursor(16,51); printf("%6.1f",spgD);           
        }
    
        // Überwachung der Grenzen
        if (temp2 > 25.0)
        {
            ds2408_lcd_dis_txt(3,0,"A-Temp zu hoch !!");

            ow_sl_aktiv = 14;

            ow_exbo_sum(1);         // Summer ein
            _wait_ms(500);          // Warten
            ow_exbo_sum(0);         // Summer aus

            ow_sl_aktiv = 15;
        }
        else
        {
            ds2408_lcd_dis_txt(3,0,"                 ");
        }

        if (spgD > 4000)
        {
            ds2408_lcd_dis_txt(3,0,"Spg-D zu hoch !! ");

            ow_sl_aktiv = 14;

            ow_exbo_sum(1);         // Summer ein
            _wait_ms(750);          // Warten
            ow_exbo_sum(0);         // Summer aus

            ow_sl_aktiv = 15;
        }
        else
        {
            ds2408_lcd_dis_txt(3,0,"                 ");
        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/