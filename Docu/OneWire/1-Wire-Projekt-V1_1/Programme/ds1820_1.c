/******************************************************************************************/
/*                                                                                        */
/*     PalmTec - Mikrocontroller-Lernsysteme                                              */
/*                                                                                        */
/*                                                                                        */
/*     Projekt:           1-Wire - DS1820-Projekt                                         */
/*                        Programm ohne Verwendung der ´ow´- und ´ds1820´-APIs            */
/*                                                                                        */
/*                                                                                        */
/*     Autoren:           B.v.B. / P.G.                                                   */
/*                                                                                        */
/*     Zielsystem:        8051er-Mikrocontroller, hier: AT89C51CC03 (Atmel)               */
/*                                                                                        */
/*                                                                                        */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                            */
/*                                                                                        */
/*     Letzte Änderung:   12.02.2010                                                      */
/*                                                                                        */
/*     Datei:             ds1820_1.c                                                      */
/*                                                                                        */
/******************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include <stdio.h>              // Standard Ein-/Ausgabefunktionen
#include <at89c51cc03.h>        // CC03er-Register

#include "cc03er.h"             // CC03er-Funktionen
#include "terminal.h"           // Terminal-Funktionen



/*******************************************************************************************/
/*** Globale Variablen *********************************************************************/
/*******************************************************************************************/

// Port-Pin zur Realisierung des 1-Wire-Busses
unsigned char bit DQ  @  0xb4;      // Port-Pin P3.4

// Gloable Arrays
unsigned char rom_c[8];             // Array zur Aufnahme des ROM-Codes

unsigned char ds1820[9];            // Array für empfangene Daten vom DS1820er: max 9 Bytes möglich


// Matrix für bis zu 5 angeschlossenen DS18S20er-Slaves:
//   -  Stations-Nummern:  0 - 4
//   -  Jeder Slave hat einen 8 Byte großen ROM-Code.
//   -  Jede Zeile in der Matrix entspricht dem ROM-Code eines Slaves.
//   -  Eintragungen fangen beim CRC-Code an, letztes Byte: Family-Code.
code unsigned char ds1820_mat[5][8] = {
{0x3f,0x00,0x08,0x01,0xdb,0x13,0x6c,0x10},
{0,0,0,0,0,0,0,0},
{0x0c,0x00,0x08,0x01,0xdb,0x08,0x21,0x10},
{0,0,0,0,0,0,0,0},
{0x54,0x00,0x08,0x01,0x86,0x70,0x0a,0x10}
};


/*******************************************************************************************/
/*** Selbstgeschriebene Funktionen *********************************************************/
/*******************************************************************************************/

/*******************************************************************************************/
/*** 1-Wire-spezifische Funktionen *********************************************************/
/*******************************************************************************************/

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



/*******************************************************************************************/
/*** DS1820-spezifische Funktionen *********************************************************/
/*******************************************************************************************/

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
/*******************************************************************************************/


/*******************************************************************************************/
/*** Applikations-spezifische Funktionen ***************************************************/
/*******************************************************************************************/

/*** Haupt-Menü-Bild ***/
void haupt_menue(void)
{
    clear_screen();

    printf("Das grosse 1-Wire - DS18S20er-Projekt\n");
    printf("=====================================\n\n");
    printf("Bitte waehlen Sie:\n\n\n");

    printf("  1. 1-Wire-Reset: Test, ob JEMAND am Bus angeschlossen ist\n\n");
    printf("  2. Single Chip: Lesen des 64 Bit ROM-Identifiers\n\n");
    printf("  3. Demo - Single Chip: Temperaturmessung und Lesen des Scratch Pads\n\n");
    printf("  4. Single Chip: 2 Byte (TH,TL) in Scratch Pad schreiben\n\n");
    printf("  5. Demo - Bus-Betrieb mit max. 5 Sensoren\n\n");
    printf("\n\n  9. Test zur uC-spezifischen Delay-Festlegung");
    printf("\n\n\n\nIhre Wahl:  ");
}

/*******************************************************************************************/

/*** Test zur uC-spezifischen Bestimmung des exakten OW-Delay-Wertes ***/

void delay_test(void)
{
    unsigned char s1[4];
    unsigned char delay,c1,i;

    while(1)
    {
        clear_screen();
        printf("Test zur uC-spezifischen Delay-Festlegung\n");
        printf("=========================================\n\n");
        printf("VOR dem Test den DS18S20 abklemmen !\n\n");
        printf("Es gibt nun zwei Moeglichkeiten, die notwendigen Zeitverzoegerungen\n");
        printf("zu erzeugen:\n\n");
        printf("  1. Mit einer reinen for-Schleife direkt im Programm-Code.\n");
        printf("  2. Durch Aufruf einer Funktion, die eine for-Schleife enthaelt.\n\n");
        printf("Die Methode 1 wird zur Erzeugung sehr kleiner Zeitverzoegerungen (< 20 us),\n");
        printf("die Methode 2 fuer groessere Verzoegerungen verwendet.\n\n");
        printf("Auswahl, was getestet werden soll (1, 2 oder 'e' (Ende)):  ");

        c1 = getchar();

        if ((c1 != '1') && (c1 != '2')) break;           // Rücksprung

        if (c1 == '1')              // Test für Methode 1
        {
            while(1)
            {
                clear_screen();
                printf("Test Methode 1: Zeitverzoegerung d. reine for-Schleife direkt im Progamm-Code\n");
                printf("=============================================================================\n");
                printf("(Ruecksprung durch Reset ... )\n\n");

                printf("\n\nTest laeuft ... ");
    
                while(1)
                {
                    DQ = 0;
                    for(i=0; i<8; i++);         // for-Schleife zur Zeitverzögerung:
                                                // Wert muss direkt eingetragen werden,
                                                // keine Variable verwenden: würde dadurch
                                                // langsamer !!!!
                    DQ = 1;
                    _wait_ms(10);
                }
            }
        }

        else                        // Test für Methode 2
        {
            while(1)
            {
                clear_screen();
                printf("Test Methode 2: Zeitverzoegerung durch Funktionsaufruf\n");
                printf("======================================================\n");
                printf("(Ruecksprung durch Eingabe von 255)\n\n");
                printf("Bitte Delay-Wert (Funktions-Uebergabe-Wert) eingeben (0 ... 254): ");
    
                inputse(s1,4);
                delay = atoi(s1);
    
                if (delay == 255) break;            // Rücksprung
    
                printf("\n\nTest laeuft, weiter mit Taste ... ");
    
                while(1)
                {
                    DQ = 0;
                    ow_delay(delay);
                    DQ = 1;
                    if (seri_zeichen_da()) break;
                    _wait_ms(10);
                }
            }
        }
    }
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
    unsigned char i,c1;

    // Endlosschleife
    while(1)
    {
        clear_screen();
        printf("Single Chip: Lesen des 64 Bit ROM-Identifiers\n");
        printf("=============================================\n");
        printf("Abfrage durch Druck auf Taste,  Beenden durch Eingabe von 'e':  ");

        // ds1820angsbuffer löschen    
        for(i=0; i<8; i++) ds1820[i]=0;
    
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

/*********************************************************************************************/

/*** DS1820: Temperaturmessung und Lesen des Scratch Pads ***/

void ds1820_get_temp(void)
{
    unsigned char i;
    float ftemp;
    unsigned int zv;
    unsigned char s1[6];

    clear_screen();
    printf("Single Chip: Temperaturmessung und Lesen des Scratch Pads\n");
    printf("=========================================================\n\n");
    printf("Bitte Abfrage-Intervall in ms eingeben (100 ... 65.000 ms):  ");

    // Zeit einlesen
    inputse(s1,6);
    zv = atoi(s1);

    // Bildschirm-Maske
    set_cursor(6,2); printf("Inhalt des Scratch Pads:");
    set_cursor(8,4); printf("Byte:     8     7     6     5     4     3     2     1     0");
    set_cursor(10,4); printf("(HEX)");

    set_cursor(14,2); printf("Aktuelle Temperatur:            C");
    set_cursor(18,0); printf("Abbruch durch Tastendruck ...");

    // ds1820angsbuffer löschen    
    for(i=0; i<9; i++) ds1820[i]=0;

    // Endlosschleife
    while (1)
    {
        // Scratch Pad des DS1820 auslesen
        ds1820_rd_temp(255);            // Adresse = 255 = nur ein Salve angeschlossen

        // Scratch Pad-Inhalt ausgeben
        set_cursor(10,14);
        printf("%02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x",ds1820[8],ds1820[7],ds1820[6],ds1820[5],ds1820[4],ds1820[3],ds1820[2],ds1820[1],ds1820[0]);

        // Temperatur berechnen, als float-Wert
        if(ds1820[1] == 0)                // Positive Temp.-Werte
        {
            ftemp = ds1820[0]/2.0;
        }
        else                            // Neg. Temp.-Werte
        {
            ftemp = (~ds1820[0])+1;       // 2er-Komplement
            ftemp = (ftemp*(-1))/2;
        }

        // Ausgabe
        set_cursor(14,26); printf("%5.1f",ftemp);

        // Abfrage auf Abbruch
        if (seri_zeichen_da()) break;

        // Warten bis zur nächsten Messung
        _wait_ms(zv);
    }
}

/*******************************************************************************************/

/*** DS1820 - Demo: Beschreiben der 2 Bytes im Scratch Pad ***/

void ds1820_scr_pa_wr_demo(void)
{
    unsigned char s1[4];
    unsigned char TH,TL,c1;

    clear_screen();
    printf("Single Chip: 2 Byte (TH,TL) in Scratch Pad schreiben\n");
    printf("====================================================\n\n");
    printf("Bitte beide Werte (0 ... 255) nachfolgend einzeln eingeben:\n\n\n");

    printf("  Wert fuer Byte 2 im Scratch Pad (TH-Register):  ");
    inputse(s1,4);
    TH = atoi(s1);

    printf("\n\n  Wert fuer Byte 3 im Scratch Pad (TL-Register):  ");
    inputse(s1,4);
    TL = atoi(s1);

    printf("\n\n\nUebertragung ins Scratch Pad durch Tastendruck ... ");
    c1 = getchar();

    // Übertragung der beiden Werte
    ds1820_wr_t_reg(TH,TL);

    printf("\n\n\nUebertragung o.k.,  Taste druecken ... ");
    c1 = getchar();
}

/*******************************************************************************************/

/*** Bemo-Betrieb mit bis zu 5 DS18S20er-Slaves ***/

void bus_demo(void)
{
    unsigned char i,merker;
    float ftemp;

    clear_screen();
    printf("1-Wire-Bus-Demo fuer max. 5 DS18S20er-Slaves\n");
    printf("============================================\n\n");
    printf("Demo laeuft ...   (Abbruch durch Tastendruck)\n\n\n");

    printf("   Slave-Nr.     Temperatur         Scratch-Pad (Byte 8 .... Byte 0)");

    // Endlosschleife
    while(1)
    {
        merker = 0;

        // Slaves abfragen
        for (i=0; i<5; i++)
        {
            // Scratch Pad des DS1820 auslesen
            ds1820_rd_temp(i);              // i = Stellung des Slaves in Matrix ds1820_mat

            // Scratch Pad-Inhalt ausgeben
            set_cursor(2*i+8,36);
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x",ds1820[8],ds1820[7],ds1820[6],ds1820[5],ds1820[4],ds1820[3],ds1820[2],ds1820[1],ds1820[0]);

            // Prüfen, ob Stat.-Nr. ok
            if (ds1820[3] != i)       // Falsche Stat.-Nr. oder Slave def.
            {
                set_cursor(2*i+8,6); printf("%u          Keine kor. Antw.",i);
                merker = 2;
            }
            else merker = 0;
            
            // Ausgabe nur, wenn Slave-Ant. korrekt war
            if (merker == 0)
            {

                // Temperatur berechnen, als float-Wert
                if(ds1820[1] == 0)                // Positve Temp.-Werte
                {
                    ftemp = ds1820[0];
                    ftemp = ftemp/2;
                }
                else                            // Neg. Temp.-Werte
                {
                    ftemp = (~ds1820[0])+1;       // 2er-Komplement
                    ftemp = (ftemp*(-1))/2;
                }
    
                // Ausgabe: Slave-Nr. und Temp.
                set_cursor(2*i+8,6); printf("%u            %5.1f         ",i,ftemp);
            }

            // Abfrage auf Abbruch
            if (seri_zeichen_da()) 
            {
                merker = 1;
                break;
            }

            // Warten bis zur nächsten Messung
            _wait_ms(50);           

        }
        if (merker == 1) break;          // Aussprung
    }
}

/********************************************************************************************/


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

            case '3':   ds1820_get_temp();
                        break;

            case '4':   ds1820_scr_pa_wr_demo();
                        break;

            case '5':   bus_demo();
                        break;

            case '9':   delay_test();
                        break;

        }

    }

}
/*******************************************************************************************/
/*** Ende des Hauptprogramms, d.h. Ende des gesamten Programms! ****************************/
/*******************************************************************************************/