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
/*     Letzte Änderung:   12.10.2010                                                        */
/*                                                                                          */
/*     Datei:             ow.c                                                              */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ow.h"
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem



/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Ausgabe einer Fehlermeldung zur einer Fehlernummer fe_nr ***/

void ow_fem(unsigned char fe_nr)
{
    printf("\n\n%3u: ",fe_nr);          // Fehlernummer ausgeben

    // Fehlernummer auswerten
    switch(fe_nr)
    {
        case 10:    printf("DS1820: Kein Pres bei ds1820_rd_temp() - 1"); break;
        case 11:    printf("DS1820: Kein Pres bei ds1820_rd_temp() - 2"); break;
        case 12:    printf("DS1820: Kein Pres bei ds1820_wr_t_reg"); break;

        case 20:    printf("DS2450: Kein Pres.: Schreiben in eine Seite (1) !"); break;
        case 21:    printf("DS2450: Fehler beim Schreiben - Ruecklesen !"); break;
        case 22:    printf("DS2450: Kein Pres.: Schreiben in eine Seite (2) !"); break;
        case 23:    printf("DS2450: Kein Pres.: Lesen aus einer Seite (1) !"); break;
        case 24:    printf("DS2450: Kein Pres.: Lesen aus einer Seite (2) !"); break;
        case 25:    printf("DS2450: Kein Pres.: Schreiben in Page 3 (Init-Anf.) !"); break;
        case 26:    printf("DS2450: Fehler beim Rücklesen von Page 3 (Init) !"); break;
        case 27:    printf("DS2450: Kein Pres.: Start Convert (Anfang) !"); break;
        case 28:    printf("DS2450: Kein Pres.: Start Convert (Ende) !"); break;
        case 29:    printf("DS2450: Kein Pres.: Schreiben in Page 3 (Init-Ende) !"); break;

        case 50:    printf("DS2408: Kein Pres bei ds2408_init()"); break;
        case 51:    printf("DS2408: Fehler bei ds2408_init()"); break;
        case 52:    printf("DS2408: Kein Pres bei ds2408_wr_byte()"); break;
        case 53:    printf("DS2408: Confirmation-Byte falsch in ds2408_wr_byte()"); break;        
        case 54:    printf("DS2408: Kein Pres bei ds2408_rd_zustand()"); break;
        case 55:    printf("DS2408: Kein Pres bei ds2408_rd_fla()"); break;
        case 56:    printf("DS2408: Kein Pres bei ds2408_fla_cl()"); break;


        default:    printf("Ungueltige Fehlernummer aufgetreten !"); break;

    }

    printf("\n\nSystem angehalten !\n\n\a");

    // Endlosschleife
    while(1);

}

/*************************************************************************************/

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

/*** Master-Reset senden und Slave-Presence abfragen - mit Fehlernummer, wird übergeben ***/

void ow_pres_check(unsigned char fe_nr)
{
    // Start mit Master-Reset-Impuls u. Abfrage: Slave-Presence
    if(ow_reset())          // Fehler aufgetreten
    {
        ow_fem(fe_nr);      // Fehlermeldung ausgeben
    }
}

/********************************************************************************************/

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

/*** Adressierung eines 1-Wire-Slaves über Skip-ROM oder Match-ROM ***/

void ow_match(void)
{
    unsigned char i;

    // Prüfen der Slave-Adresse
    if(ow_sl_aktiv == 255)                  // Nur ein Slave am Bus angeschlossen,
                                            // also mit Skip-ROM arbeiten
    {
        // Skip ROM-Befehl, da nur ein Slave angeschlossen ist
        ow_wr_byte(0xcc);
    }
    else                                    // Mehrere Slaves am Bus
    {
        // Match ROM-Befehl und Slave-ROM-Code aussenden.
        // ROM-ID gem. Eintrag in Slave-Matrix ´ow_sl_mat[..]´ in
        // der Header-Datei ´ow_slaves.h´
        // Die globale Var. ´ow_sl_aktiv´ enthält die Nr. des 1-Wire-Slaves,
        // mit dem gearbeitet werden soll !

        ow_wr_byte(0x55);                   // Match-ROM-Anweisung

        // Entspr. ROM-ID aussenden
        for (i=0; i<8; i++) ow_wr_byte(ow_sl_mat[ow_sl_aktiv][7-i]);
    }

    // Ab hier kommt dann der jeweilige Befehl für den 1-Wire-Slave:
    // ist dann die Fortsetzung im aufrufenden Programm-Teil !

}

/******************************************************************************************/


/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

