/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus - DS18S20:                                                                */
/*     =====================                                                                */
/*                                                                                          */
/*     Sammlung der API-Funktionen zum Betrieb des digitalen Temperatursensors DS18S20      */
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
/*     Letzte Änderung:   03.09.2010                                                        */
/*                                                                                          */
/*     Datei:             ds1820.c                                                          */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ds1820.h"
#include "ow.h"   // 1-Wire-API


/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Übertragung von 2 Bytes in die Register TH und TL des Scratch Pads ***/

void ds1820_wr_t_reg(unsigned char TH, unsigned char TL)
{
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(12);

    /********************************************/
    /*** 1. Werte in Scratch Pad einschreiben ***/
    /********************************************/

    // Adressierung des DS1820ers
    ow_match();

    // Schreib-Befehl senden: "WRITE SCRATCHPAD" = 0x4e
    ow_wr_byte(0x4e);

    // Byte 1 senden: Reg TH
    ow_wr_byte(TH);

    // Byte 2 senden: Reg TL
    ow_wr_byte(TL);

    // Msster sendet abschließenden Reset-Impuls
    // 1. Befehlszyklus: Messung starten    
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(12);


    /****************************************************/
    /*** 2. Werte vom Scratch Pad ins EEPROM kopieren ***/
    /****************************************************/

    // Adressierung des DS1820ers
    ow_match();

    // Kopieren ins Scratch Pad (EEPROM)-Befehl senden: "COPY SCRATCHPAD" = 0x48
    ow_wr_byte(0x48);   

    _wait_ms(15);           // Wartezeit f. EPROM-Programmierung
}

/*******************************************************************************************/

/*** Abfrage eines DS18S20ers: Auslesen des Scratch Pad u. Berechnung d. Temperatur ***/

float ds1820_rd_temp(void)
{
    unsigned char i;
    float ftemp;
    unsigned int sum;

    // 1. Befehlszyklus: Messung starten    
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(10);

    // Adressierung des DS1820ers
    ow_match();

    // Befehl: Start Conversion an DS1820
    ow_wr_byte(0x44);

    _wait_ms(800);          // Warte-Zeit, da Chip eine max. Conversion Time
                            // von 750 ms hat !


    // 2. Befehlszyklus: Messwerte (Scratch Pad) auslesen    
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    ow_pres_check(11);

    // Adressierung des DS1820ers
    ow_match();

    // Befehl: Auslesen des Scatch Pads vom DS1820
    ow_wr_byte(0xbe);

    // Antwort einlesen: 9 Byte großen Scratch Pad-Inhalt einlesen
    for (i=0; i<9; i++)
    {
        ds1820[i] = ow_rd_byte();
    }

    // Temperatur berechnen, als float-Wert
    if(ds1820[1] == 0)              // Positive Temp.-Werte
    {
        ftemp = ds1820[0]/2.0;
    }
    else                            // Neg. Temp.-Werte
    {
        ftemp = (~ds1820[0])+1;     // 2er-Komplement
        ftemp = (ftemp*(-1))/2;
    }

    // Fehler-Prüfung: Scrartch-Pad-Inhalt aufaddieren
    sum = 0;    // Fehler-Summe löschen
    for(i=0; i<9; i++) sum = sum + ds1820[i];
    if(sum == 2295) ftemp = 555;    

    // Rückgabe
    return(ftemp);
}

/********************************************************************************/

/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

