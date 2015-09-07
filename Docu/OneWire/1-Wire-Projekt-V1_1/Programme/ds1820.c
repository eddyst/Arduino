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
/*     Letzte Änderung:   12.02.2010                                                        */
/*                                                                                          */
/*     Datei:             ow.c                                                              */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ds1820.h"
#include "ow.h"			// 1-Wire-API


/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Übertragung von 2 Bytes in die Register TH und TL des Scratch Pads ***/

void ds1820_wr_t_reg(unsigned char TH, unsigned char TL)
{
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    if(ow_reset()) 
    {
        printf("\n\n\n\n  Fehler beim Schreiben des Scratch Pads 1: KEIN Presence vom Slave !");
        printf("\n\n  Reset druecken ... ");
        while(1);
    }

    /********************************************/
    /*** 1. Werte in Scratch Pad einschreiben ***/
    /********************************************/

    // Skip ROM-Befehl, da nur ein DS1820 angeschlossen ist
    ow_wr_byte(0xcc);

    // Schreib-Befehl senden: "WRITE SCRATCHPAD" = 0x4e
    ow_wr_byte(0x4e);

    // Byte 1 senden: Reg TH
    ow_wr_byte(TH);

    // Byte 2 senden: Reg TL
    ow_wr_byte(TL);

    // Msster sendet abschließenden Reset-Impuls
    if(ow_reset()) 
    {
        printf("\n\n\n\n  Fehler beim Schreiben des Scratch Pads 2: KEIN Presence vom Slave !");
        printf("\n\n  Reset druecken ... ");
        while(1);
    }

    /****************************************************/
    /*** 2. Werte vom Scratch Pad ins EEPROM kopieren ***/
    /****************************************************/

    // Skip ROM-Befehl, da nur ein DS1820 angeschlossen ist
    ow_wr_byte(0xcc);

    // Kopieren ins Scratch Pad (EEPROM)-Befehl senden: "COPY SCRATCHPAD" = 0x48
    ow_wr_byte(0x48);   

    _wait_ms(15);           // Wartezeit f. EPROM-Programmierung
}

/*******************************************************************************************/

/*** Abfrage eines DS18S20ers mit der Slave-Adresse s_adr ***/

void ds1820_rd_temp(unsigned char s_adr)
{
    unsigned char i;

    // 1. Befehlszyklus: Messung starten    
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    if(ow_reset()) 
    {
        printf("\n\n\n\n  Fehler beim 1. Reset: KEIN Presence vom Slave !");
        printf("\n\n  Reset druecken ... ");
        while(1);
    }

    // Prüfen der Slave-Adresse
    if(s_adr == 255)                        // Nur ein Slave am Bus angeschlossen
    {
        // Skip ROM-Befehl, da nur ein DS1820 angeschlossen ist
        ow_wr_byte(0xcc);
    }
    else                                    // Mehrere Slaves am Bus
    {
        // Match ROM-Befehl und ROM-Code aussenden
        ow_wr_byte(0x55);
        for (i=0; i<8; i++) ow_wr_byte(ds1820_mat[s_adr][7-i]);
    }

    // Befehl: Start Conversion an DS1820
    ow_wr_byte(0x44);

    _wait_ms(800);          // Warte-Zeit, da Chip eine max. Conversion Time
                            // von 750 ms hat !
    

    // 2. Befehlszyklus: Messwerte (Scratch Pad) auslesen    
    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    if(ow_reset()) 
    {
        printf("\n\n\n\n  Fehler beim 2. Reset: KEIN Presence vom Slave !");
        printf("\n\n  Reset druecken ... ");
        while(1);
    }

    // Prüfen der Slave-Adresse
    if(s_adr == 255)                        // Nur ein Slave am Bus angeschlossen
    {
        // Skip ROM-Befehl, da nur ein DS1820 angeschlossen ist
        ow_wr_byte(0xcc);
    }
    else                                    // Mehrere Slaves am Bus
    {
        // Match ROM-Befehl und ROM-Code aussenden
        ow_wr_byte(0x55);
        for (i=0; i<8; i++) ow_wr_byte(ds1820_mat[s_adr][7-i]);
    }

    // Befehl: Auslesen des Scatch Pads vom DS1820
    ow_wr_byte(0xbe);

    // Antwort einlesen: 9 Byte große Scratch Pad-Inhalt einlesen
    for (i=0; i<9; i++)
    {
        ds1820[i] = ow_rd_byte();
    }
}

/*******************************************************************************************/


/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

