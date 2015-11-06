/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Sammlung der API-Funktionen speziell zum Betrieb des                   */
/*                   PT-1-Wire-ExBo                                                         */
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
/*     Datei:             ow_exbo.c                                                         */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ow.h"
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem
#include "ds2408.h"             // DS2408er-API

#include "ow_exbo.h"            // Diese API



/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** PT-1-Wire-ExBo-Funktion: gezielt einen Port-Pin setzen/zurücksetzen ohne die ***/
/*** anderen Port-Pins zu beeinflussen                                            ***/
/*** Besonderheit wg. Taster an den Pins PIO0 - PIO3                              ***/

void ow_exbo_set_port(unsigned char nr,unsigned char zustand)
{
    unsigned char zw,maske;

    // Aktuellen Port-Zustand einlesen
    zw=ds2408_rd_zustand();

    // Port-Pin setzen/rücksetzen
    if (zustand)        // Auf 1 setzen
    {
        maske = 1 << nr;
        zw = zw | maske;
    }
    else                // Auf 0 setzen
    {
        maske = 0xff - (1 << nr);
        zw = zw & maske;
    }

    // PT-1Wire-ExBo-spezifisch:
    // Neues Ausgabe-Byte anpassen wg. Taster an PIO0-PIO3:
    // Port-Pins auf log.´1´ setzen !
    zw = zw | 0x0f;

    // Neuen Port-Zustand wieder ausgeben
    ds2408_wr_byte(zw);
}

/******************************************************************************************/

/*** PT-1-Wire-ExBo-Funktion: Relais ein- und ausschalten ***/

void ow_exbo_rel(unsigned char nr, unsigned char zustand)
{
    // Ansteuerung des zugehörigen Port-Pins
    ow_exbo_set_port((nr+4),(1-zustand));       // Relais aktiv mit Low-Pegel !
}

/******************************************************************************************/

/*** PT-1-Wire-ExBo-Funktion: Summer ein- und ausschalten ***/

void ow_exbo_sum(unsigned char zustand)
{
    // Ansteuerung des zugehörigen Port-Pins: PIO 7
    ow_exbo_set_port(7,(1-zustand));       // Summer aktiv mit Low-Pegel !
}

/******************************************************************************************/

/*** PT-1-Wire-ExBo-Funktion: Scannen der Tasten auf dem ExBo ***/

unsigned char ow_exbo_ta_in(void)
{
    unsigned char zw;

    // Gesamten PIO-Port einlesen
    zw=ds2408_rd_zustand();

    // Tasten-Bits selektieren
    zw= zw & 0x0f;

    // Tasten-Bits auswerten und Rückgabe
    switch(zw)
    {
        case 15:    return(0xff); break;        // Keine Taste gedrückt
        case 14:    return(0); break;           // Taste 0
        case 13:    return(1); break;           // Taste 1
        case 11:    return(2); break;           // Taste 2
        case 7:     return(3); break;           // Taste 3
    }

    return(0xff);       // Dummy-Return, wg. Compiler-Warning
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




/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

