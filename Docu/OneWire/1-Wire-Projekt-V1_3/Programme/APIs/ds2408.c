/********************************************************************************************/
/*                                                                                          */
/*     PalmTec - Mikrocontroller-Lernsysteme                                                */
/*                                                                                          */
/*     1-Wire-Bus:   Sammlung der API-Funktionen zum Betrieb des 1-Wire-Bausteins           */
/*                   DS2408 = 8-fach dig. I/O (PIO)                                         */
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
/*     Datei:             ds2408.c                                                          */
/*                                                                                          */
/********************************************************************************************/


/*** Einbinden von Standard-Include-Dateien ***/
#include <stdio.h>

/*** Einbinden eigener Funktionssammlungen ***/
#include "ow.h"
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem

#include "ds2408.h"             // Diese API


/******************************************************************************/
/*** Sekbstgeschriebene Funktionen in der API *********************************/
/******************************************************************************/

/*** Init. des DS2408er: Einschreiben eines Byte ins Control/Status-Reg ***/

void ds2408_init(unsigned char dat)
{
    unsigned char zw;

    /************************************************/
    /*** Wert in Control/Status-Reg. einschreiben ***/
    /************************************************/

    // Master Reset / Slave Presensce
    ow_pres_check(50);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Write Conditional Search Register: cch
    ow_wr_byte(0xcc);

    // Adresse des Zielregs = Control/Status-Reg übermitteln als 2 Byte Adr.: 008d
    ow_wr_byte(0x8d);
    ow_wr_byte(0x00);

    // Wert für das Register übermitteln: dat
    ow_wr_byte(dat);

    /**************************************************************/
    /*** Wert aus Control/Status-Reg. zur Kontrolle zurücklesen ***/
    /**************************************************************/

    // Master Reset / Slave Presensce
    ow_pres_check(50);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Read PIO Registers: f0h
    ow_wr_byte(0xf0);

    // Adresse des Zielregs = Control/Status-Reg übermitteln als 2 Byte Adr.: 008d
    ow_wr_byte(0x8d);
    ow_wr_byte(0x00);  

    // Wert einlesen
    zw=ow_rd_byte();

    // Master beendet Komm-Zyklus mit Reset
    ow_pres_check(50);

    // Vergleich: gesendeter / empfangener Wert (+80h)
    if (zw != (dat + 0x80)) ow_fem(51);
}

/******************************************************************************************/

/*** Schreiben eines Bytes (8 Bits) in den DS2408er ***/

void ds2408_wr_byte(unsigned char dat)
{
    unsigned char zw;

    /*************************************************/
    /*** Wert in PIO-Ausgabe-Reg. 89h einschreiben ***/
    /*************************************************/

    // Master Reset / Slave Presence
    ow_pres_check(52);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Channel Access Write: 5ah
    ow_wr_byte(0x5a);

    // Ausgabe des Bitmusters (Byte)
    ow_wr_byte(dat);

    // Ausgabe des invertierten Bitmusteres (Bytes)
    ow_wr_byte(~dat);

    // Warten auf positive Quitungs-Rückantwort vom DS2408er: aah
    // ev. Wartezeit einbauen ????
    if (ow_rd_byte() != 0xaa) ow_fem(53);

    // Einlesen des gesetzten Zustandes von der PIO
    zw=ow_rd_byte();

    // Master beendet Komm-Zyklus mit Reset
    ow_pres_check(52);

    // Vergleich: empfangener Zustand / gesendeter Zustand
    // NICHT notwendig !  s. eigene Ausführungen

}

/******************************************************************************************/

/*** Einlesen der aktuellen Zustände an den PIO-Port-Pins ***/

unsigned char ds2408_rd_zustand(void)
{
    unsigned char zw,i,zw1;

    /**************************************************/
    /*** Wert aus PIO Logic State-Reg. 88h auslesen ***/
    /**************************************************/

    // Master Reset / Slave Presence
    ow_pres_check(54);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Read PIO Reg: f0h
    ow_wr_byte(0xf0);

    // Adresse des Zielregs = PIO Logic State-Reg übermitteln als 2 Byte Adr.: 0088h
    ow_wr_byte(0x88);
    ow_wr_byte(0x00);  

    // Wert einlesen aus Reg 88h
    zw=ow_rd_byte();

    // Dummy-Reads der restlichen Registerinhalte, um das CRC16\ zu erhalten
    // s. Datenblatt S.12: noch Register 89h - 8fh auslesen: 7 Stück
    for (i=0; i<7; i++)
    {
        zw1=ow_rd_byte();       // Dummy Read
    }

    // 16-Bit-CRC vom DS2408er zurück lesen
    crc1=ow_rd_byte();              // CRC-1
    crc2=ow_rd_byte();              // CRC-2

    // CRC16\ wird hier z.Z. nicht weiter ausgewertet !

    // Komm-Zyklus beenden durch Master Reset / Slave Presence
    ow_pres_check(54);

    // Rückgabe des gewünschten Wertes aus Reg 88h
    return(zw);
}

/*****************************************************************************************/

/*** Setzen / Rücksetzen eines einzelnen PIO-Port-Pins ***/

void ds2408_set_port(unsigned char nr, unsigned char zustand)
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

    // Neuen Port-Zustand wieder ausgeben
    ds2408_wr_byte(zw);
}

/*****************************************************************************************/

/*** Einlesen des Zustandes eines einzelnen Port-Pins ***/

unsigned char ds2408_get_port(unsigned char nr)
{
    unsigned char zw,maske;

    // Aktuellen Port-Zustand einlesen
    zw=ds2408_rd_zustand();

    // Gewünschten Port maskieren
    maske = 1 << nr;
    zw = zw & maske;

    // Rückgabe
    if (zw == 0) return(0);
    else return(1);
}

/****************************************************************************************/

/*** Einlesen der aktuellen Zustände der Flanken-Wechsel-FFs der PIO-Port-Pins ***/

unsigned char ds2408_rd_fla(void)
{
    unsigned char zw,i,zw1;

    /***********************************************************/
    /*** Wert aus PIO Activity Latch State-Reg. 8ah auslesen ***/
    /***********************************************************/

    // Master Reset / Slave Presence
    ow_pres_check(55);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Read PIO Reg: f0h
    ow_wr_byte(0xf0);

    // Adresse des Zielregs = PIO Activity Latch State-Reg übermitteln als 2 Byte Adr.: 008ah
    ow_wr_byte(0x8a);
    ow_wr_byte(0x00);  

    // Wert einlesen aus Reg 8ah
    zw=ow_rd_byte();

    // Dummy-Reads der restlichen Registerinhalte, um das CRC16\ zu erhalten
    // s. Datenblatt S.12: noch Register 8bh - 8fh auslesen: 5 Stück
    for (i=0; i<5; i++)
    {
        zw1=ow_rd_byte();       // Dummy Read
    }

    // 16-Bit-CRC vom DS2408er zurück lesen
    crc1=ow_rd_byte();              // CRC-1
    crc2=ow_rd_byte();              // CRC-2

    // CRC16\ wird hier z.Z. nicht weiter ausgewertet !

    // Komm-Zyklus beenden durch Master Reset / Slave Presence
    ow_pres_check(55);

    // Rückgabe des gewünschten Wertes aus Reg 88h
    return(zw);
}

/*****************************************************************************************/

/*** Reset der Flanken-Wechsel FFs des DS2408er (Reg. 8ah = PIO Activity Latch State) ***/

void ds2408_fla_cl(void)
{

    /****************************************************************/
    /*** Rücksetz-Befehl übermitteln: Reset Activity Latches: c3h ***/
    /****************************************************************/

    // Master Reset / Slave Presence
    ow_pres_check(56);

    // Adressierung des DS2408ers
    ow_match();

    // Kommando an DS2408: Reset Activity Latches: c3h
    ow_wr_byte(0xc3);

    // Warten auf Quittungs-Rückantwort vom DS2408er: aah
    while (ow_rd_byte() != 0xaa);

    // Master beendet Komm-Zyklus mit Reset
    ow_pres_check(56);
}

/*******************************************************************************************/

/*** Einlesen des Zustandes eines einzelnen Flanken-Wechsel-FF eines Port-Pins ***/

unsigned char ds2408_get_fla(unsigned char nr)
{
    unsigned char zw,maske;

    // Aktuellen Flanken-Wechsel-Zustand einlesen
    zw=ds2408_rd_fla();

    // Gewünschten Port maskieren
    maske = 1 << nr;
    zw = zw & maske;

    // Rückgabe
    if (zw == 0) return(0);
    else return(1);
}

/****************************************************************************************/


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

