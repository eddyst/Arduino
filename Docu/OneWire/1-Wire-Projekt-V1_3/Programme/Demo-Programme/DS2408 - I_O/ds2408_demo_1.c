/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS2408-Projekt - Digital I/O                           */
/*                        Programm unter Verwendung der 1-Wire-APIs:                      */
/*                           -  ow.h                                                      */
/*                           -  ow_slaves.h                                               */
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
/*     Datei:             ds2408_demo_1.c                                                 */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen

#include "ow.h"                 // 1-Wire-Bus-API
#include "ow_slaves.h"          // Adressen der 1-Wire-Slaves im Gesamtsystem


/*******************************************************************************************/
/*** Globale Variablen *********************************************************************/
/*******************************************************************************************/






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

    printf("Das grosse 1-Wire - DS2408er-Projekt - Digital I/O\n");
    printf("==================================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. Ausgabe eines Byte-Wertes\n\n");
    printf("  4. Blinken eines Port-Pins\n\n");
    printf("  5. Einlesen des gesamten Port-Zustandes\n\n");
    printf("  6. Zustand eines Port-Pins einlesen\n\n");
    printf("\n\n  9. ");
    printf("\n\n\nIhre Wahl:  ");
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
/*** DS2408er-spezifische Funktionen *******************************************************/
/*******************************************************************************************/

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

/*******************************************************************************************/
/*** PT-ONE-Wire-ExBo-spezifische Funktionen ***********************************************/
/*******************************************************************************************/

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

/*******************************************************************************************/
/*** Demo-Funktionen ***********************************************************************/
/*******************************************************************************************/

/*** Byte-Werte ausgeben ***/

void byte_aus(void)
{
    unsigned char s1[4];
    unsigned char wert;

    // Endloschleife
    while(1)
    {
        // Bildschirm löschen
        clear_screen();
    
        printf("Ausgabe eines Byte-Wertes\n");
        printf("==========================\n\n");
        printf("(Ruecksprung durch Eingabe von 255)\n\n\n");
        printf("  Byte-Wert in DEZ (0 .. 255):   ");
    
        // Byte-Wert einlesen
        inputse(s1,4);
    
        // Byte-Wert wandeln
        wert=atoi(s1);

        // Byte-Wert aussenden
        ds2408_wr_byte(wert);
    
        // Rücksprung
        if (wert == 0xff) break;

        // Löschen
        set_cursor(6,33); printf("  "); set_cursor(6,33);
    }
}

/*******************************************************************************************/

/*** Blinken eines Port-Pins ***/

void port_pin_blink(void)
{
    unsigned char nr;
    unsigned int zv;
    unsigned char s1[6];

    // Bildschirm löschen
    clear_screen();
    
    printf("Blinken eines Port-Pins\n");
    printf("=======================\n\n");
    printf("(Ende durch Tastendruck,  Ruecksprung durch Eingabe von 'e')\n\n\n");
    printf("  Port-Pin-Nr. (0 .. 7):   ");

    // Endloschleife
    while(1)
    {
        // Port-Pin-Nr. einlesen
        nr=getchar();
        if (nr == 'e') break;
        nr=nr-0x30; printf("%u",nr);
    
        printf("\n\n  Eingabe der Zeitverzoegerung in ms (1 ... 65535):  ");

        // Wert einlesen
        inputse(s1,6);

        // Byte-Wert wandeln
        zv=atoi(s1); if(zv == 0) zv=1;

        // Port-Pin blinken lassen
        while(1)
        {
            ds2408_set_port(nr,0);      // Port-Pin auf Low
            _wait_ms(zv);
            ds2408_set_port(nr,1);      // Port-Pin auf High
            _wait_ms(zv);

            // Aussprung
            if(seri_zeichen_da()) break;
        }

        // Löschen
        set_cursor(8,52); printf("       ");
        set_cursor(6,27); printf(" "); set_cursor(6,27);
    }
}

/*******************************************************************************************/

/*** Einlesen des gesamten Port-Zustandes ***/

void read_port(void)
{
    unsigned int zw;

    // Bildschirm löschen
    clear_screen();
    
    printf("Einlesen des gesamten Port-Zustandes\n");
    printf("====================================\n\n");
    printf("(Ende durch Tastendruck)\n\n\n");
    printf("  Port-Zustand in HEX  /  DEZ:");

    // Endloschleife
    while(1)
    {
        // Port-Zustand einlesen
        zw = ds2408_rd_zustand();

        // Ausgabe
        set_cursor(6,33);
        printf("%2x  /  %3u",zw,zw);

        // Aussprung
        if(seri_zeichen_da()) break;
    }
}

/*******************************************************************************************/

/*** Zustand eines Port-Pins einlesen ***/

void read_port_pin(void)
{
    unsigned char nr,zw;

    // Bildschirm löschen
    clear_screen();
    
    printf("Zustand eines Port-Pins einlesen\n");
    printf("================================\n\n");
    printf("(Ende durch Tastendruck,  Ruecksprung durch Eingabe von 'e')\n\n\n");
    printf("  Port-Pin-Nr. (0 .. 7):\n\n");
    printf("  Zustand:");

    // Endloschleife
    while(1)
    {
        // Port-Pin-Nr. einlesen
        set_cursor(6,27);
        nr=getchar();
        if (nr == 'e') break;
        nr=nr-0x30; printf("%u",nr);
    
        // Zustand des Port-Pins einlesen und darstellen
        while(1)
        {
            // Zustand einlesen
            zw=ds2408_get_port(nr);

            // Darstellen
            set_cursor(8,12); printf("%1u",zw);

            // Aussprung
            if(seri_zeichen_da()) break;
        }

        // Löschen
        set_cursor(8,12); printf(" ");
        set_cursor(6,27); printf(" "); set_cursor(6,27);
    }
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
    unsigned char c1;

    // Initialisierung der ser. Schnittstelle des CC03ers
    seri_init();

    // Bildschirm löschen
    clear_screen();

    // Wartezeit für 1-Wire-Slaves
    _wait_ms(100);

    // Den aktiven 1-Wire-Slave für die nachfolgende Kommunikation festlegen
    ow_sl_aktiv = 14;       // Slave Nr 14: DS2450er - 1 - Digital I/O

    // Initialisierung des DS2408ers: Übergabe des entspr. Bytes fürs Control-Reg.
    ds2408_init(0x04);      // RSTZ-Pin als STRB\-Ausgang

    // Flanken-Wechsel-FFs zurücksetzen
    ds2408_fla_cl();

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

            case '3':   byte_aus();         // Byte-Wert ausgeben
                        break;

            case '4':   port_pin_blink();   // Blinken eines Port-Pins
                        break;

            case '5':   read_port();        // Einlesen des gesamten Port-Zustande
                        break;

            case '6':   read_port_pin();    // Zustand eines Port-Pins einlesen
                        break;            

            case '9':   ;
                        break;

        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/