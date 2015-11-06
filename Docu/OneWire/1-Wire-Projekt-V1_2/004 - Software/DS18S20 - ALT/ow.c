/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Sammlung der API-Funktionen zum grundlegenden Betrieb des              */
/*                   1-Wire-Busses                                                          */
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
#include "ow.h"


/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*************************************************************************************/
/*** Basis 1-Wire-Funktionen *********************************************************/
/*************************************************************************************/

/*** Realisierung des 1-Wire-Delays ***/

void ow_delay(unsigned char delay)
{
    unsigned char i;

    for(i=0; i<delay; i++);

}

/*******************************************************************************************/

/*** 1-Wire-Reset ***/

unsigned char ow_reset(void)
{
    unsigned char zw;

    DQ = 0;                 // DQ auf Low
    ow_delay(73);           // 496 us warten
    DQ = 1;                 // DQ auf High
    ow_delay(7);            // 65 us warten
    zw = DQ;                // Zustand DQ einlesen
    ow_delay(62);           // 424 us warten = Ende des Reset-Vorgangs
    return(zw);             // Rueckgabe: 0 = Slave vorhanden, 1 = kein Slave vorhanden
}

/*******************************************************************************************/

/*** Ausgabe eines Bits über den 1-Wire-Bus ***/

void ow_wr_bit(unsigned char bitwert)
{
    DQ = 0;                         // Start Time Slot: DQ auf Low
    if(bitwert) DQ = 1;             // Bei log´1´: sofort wieder
                                    // auf High = nur kurzer Low-Impuls
    ow_delay(13);                   // ca. 105 us warten bis
                                    // Ende des Time Slots
    DQ = 1;                         // DQ wieder auf High
}

/*******************************************************************************************/

/*** Einlesen eines Bits über den 1-Wire-Bus ***/

unsigned char ow_rd_bit(void)
{
    unsigned char zw;
    unsigned char i;

    DQ = 0;                     // DQ auf Low
    DQ = 1;                     // DQ sofort wieder High
    for(i=0; i<6; i++);         // 15 us warten
    zw = DQ;                    // DQ einlesen und speichern
    ow_delay(13);               // noch 105 us warten
                                // bis Ende Time Slot
    return(zw);                 // Rückgabe von DQ
}

/*******************************************************************************************/

/*** Ausgabe eines Bytes über den 1-Wire-Bus ***/

void ow_wr_byte(unsigned char dat)
{
    unsigned char i;
    unsigned char maske = 1;

    // 8 Bits nacheinander raus schieben, LSB zuerst
    for (i=0; i<8; i++)
    {
        if (dat & maske) ow_wr_bit(1);          // log.´1´ senden
        else ow_wr_bit(0);                      // log.´0´ senden
        maske = maske * 2;                      // nächstes Bit selektieren
    }
}

/*******************************************************************************************/

/*** Einlesen eines Bytes über den 1-Wire-Bus ***/

unsigned char ow_rd_byte(void)
{
    unsigned char i;
    unsigned char wert = 0;

    // 8 Bits hintereinander einlesen, LSB zuerst
    for(i=0; i<8; i++)
    {
        if (ow_rd_bit()) wert |=0x01 << i;
    }

    return(wert);
}

/*******************************************************************************************/


/********************************************************************************************/
/*** Höherwertige 1-Wire-Funktionen *********************************************************/
/********************************************************************************************/

/*** Lesen des 64-Bit-ROM-Identifiers ***/

void ow_rd_rom(void)
{
    unsigned char i;

    // Start mit Master-Reset-Impuls u. Abfrage: Slave presence
    if(ow_reset()) 
    {
        printf("\n\n\n\n  Fehler beim Lesen des ROM-Identifiers: KEIN Presence vom Slave !");
        printf("\n\n  Reset druecken ... ");
        while(1);
    }

    // Abfrage-Befehl senden: "READ ROM" = 0x33
    ow_wr_byte(0x33);

    // Antwort einlesen: 8 Byte = 64 Bit Identifier ins globale Array rom_c[.]
    for (i=0; i<8; i++)
    {
        rom_c[i] = ow_rd_byte();
    }
}

/********************************************************************************************/


/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

