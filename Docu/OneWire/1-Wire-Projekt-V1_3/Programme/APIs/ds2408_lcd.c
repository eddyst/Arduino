/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Sammlung der API-Funktionen zum Betrieb eines LC-Displays              */
/*                   am DS2408                                                              */
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
/*     Datei:             ds2408_lcd.c                                                      */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ow.h"
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem

#include "ds2408.h"             // DS2408er-API
#include "ds2408_lcd.h"         // Diese API



/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Ein Byte, gesplittet in zwei Nibbles, in ein LCD-Register einschreiben ***/

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


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

