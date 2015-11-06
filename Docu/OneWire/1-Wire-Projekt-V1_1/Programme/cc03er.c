/********************************************************************************************/
/*                                                                                          */
/*     TFH Georg Agricola, Bochum   /   Elektor-Verlag, Aachen                              */
/*                                                                                          */
/*     Fernlehrgang-Funktionssammlung, Grundfunktionen zum Betrieb der ON-Chip seriellen    */
/*     Schnittstelle und des ON-Chip A/D-Wandlers des AT89C51CC03ers:                       */
/*                                                                                          */
/*     Funktionen:                                                                          */
/*     seri_init        :=  Initialisierung der seriellen Schnittstelle des CC03ers!        */
/*     seri_empf        :=  Wartet, bis ein Zeichen empfangen wurde und gibt dieses zur�ck! */
/*     seri_send        :=  Sendet ein Zeichen direkt �ber dieSchnitstelle des CC03ers aus! */
/*     seri_zeichen_da  :=  Fragt die Schnittstelle ab, ob ein Zeichen empfangen wurde!     */
/*     mw_erf8          :=  Betrieb des A/D-Wandlers, Funktion liefert 8-Bit Ergebnis!      */
/*     mw_erf10         :=  Betrieb des A/D-Wandlers, Funktion liefert 10-Bit Ergebnis!     */
/*                                                                                          */
/*     Autoren:           P.G. / B.v.B.                                                     */
/*                                                                                          */
/*     Zielsystem:        TFH-Core ONE, Prozessor: AT89C51CC03 (Atmel)                      */
/*                                                                                          */
/*                                                                                          */
/*     IDE/C-Compiler:    uC/51 (Wickenh�user)                                              */
/*                                                                                          */
/*     Letzte �nderung:   20.12.2008                                                        */
/*                                                                                          */
/*     Datei:             cc03er.c                                                          */
/*                                                                                          */
/********************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include<at89c51cc03.h>     // Definition der SFRs des Mikrocontrollers        

/********************************************************************************/
/*  Die Funktion void seri_init(void) initialisiert die ser. Schnittstelle des  */
/*  CC03ers bei einer Taktfrequenz von 11,0592MHz mit nachfolgenden Schnitt-    */
/*  stellen Parametern: 9600 Bd, 1 Stopp-Bit, keine Parit�t, asynchr. Betrieb!  */
/********************************************************************************/

void seri_init(void)
  {
    SCON=0x52;
    TMOD |=0x20;
    TH1=0xfd;                      
    TR1=1;
    TI=1;
  }

/********************************************************************************/
/*  Die Funktion unsigned char seri_empf(void) wartet solange, bis ein Zeichen  */
/*  (Datenbyte) �ber die serielle Schnittstelle des CC03ers empfangen wurde.    */
/*  Der ASCII-Code des empfangenen Zeichens als Funktionswert zur�ckgegeben!    */
/********************************************************************************/

unsigned char seri_empf(void)
  {
    while(RI == 0);     // Warte, bis Zeichen empfangen wurde, also RI log 1 wird!
    RI = 0;             // Nach dem Empfang RI wieder zur�cksetzen!
    return(SBUF);       // Empfangenes Zeichen zur�ckgeben!
  }

/********************************************************************************/
/*  Funktion void seri_send(unsigned char DATEN) sendet ein Datenbyte           */
/*  (Parameter DATEN) �ber die serielle Schnittstelle des CC03ers aus.          */
/********************************************************************************/

void seri_send(unsigned char DATEN)
  {
    while(TI == 0);     // Warte, bis Schnittstelle bereit ist, also TI log. 1 ist!
    TI = 0;             // TI auf log. 0 zur�cksetzen, nach dem Aussenden des komp-
                        // letten Bytes wird TI von der Hardware auf log. 1 gesetzt!
    SBUF = DATEN;       // Auszusendendes Byte in das SBUF-SFR schreiben!
  }

/********************************************************************************/
/*  Die Funktion unsigned char seri_zeichen_da(void) fragt die serielle         */
/*  Schnittstelle des CC03ers ab, ob ein Zeichen empfangen wurde.               */
/*  Ist dies der Fall, so wird der ASCII-Code des Zeichens �bergeben,           */
/*  wenn kein Zeichen empfangen wurde, wird 0 zur�ckgegeben!                    */
/********************************************************************************/

unsigned char seri_zeichen_da(void)
  {
    unsigned char z=0;      // Merker zur�cksetzen
    if (RI==1)
      {
        RI=0;               // RI zur�cksetzen
        z=SBUF;             // Zeichen einlesen
      }
    return(z);              // Zeichen zur�ck geben
  }

/********************************************************************************/
/*  Die Funktion unsigned int mw_erf8(unsigned char kanal) dient zum Betrieb    */
/*  des ON-Chip A/D-Wandlers des AT89C51CC03. Der Funktion wird der Eingangs-   */
/*  kanal �bergeben und sie liefert ein 8-Bit Wandlungsergebnis als Funktions-  */
/*  wert zur�ck.                                                                */
/*  Wichtig:        Diese Funktion setzt voraus, das die Grundinitialisierung   */
/*                  des ON-Chip A/D-Wandlers ausserhalb der Funktion, also      */
/*                  z. B. im Hauptprogramm erfolgt. Zur Grundinitialisierung    */
/*                  muss der A/D-Wandler eingeschaltet werden, also das         */
/*                  ADEN-Bit im ADCON SFR gesetz werden ( ADCON = 0x20) und     */
/*                  der zu wandelnde Eingangskanal im SFR ADCF als Analog-      */
/*                  eingang definiert werden (z. B. ADCF = 0xff ->              */
/*                  gesamter Port 1 als Analogeingang definiert!                */
/*  Parameter kanal := Auswahl des A/D-Wandler Eingangskanals (0..7)            */
/********************************************************************************/

unsigned char mw_erf8(unsigned char kanal)
  {      
    ADCON = (ADCON & 0xe8);                 // Kanalauswahl und Wandlungsende-Bit r�cksetzen!
    ADCON = (ADCON | (0x08+kanal));         // Kanalauswahl und Start der einmaligen Wandlung!
    while((ADCON & 0x10) == 0);             // Warte bis A/D-Wandlung beendet ist!
    return(ADDH);                           // Ergebnisregister auslesen und �bergeben!
  }

/********************************************************************************/
/*  Die Funktion unsigned int mw_erf10(unsigned char kanal) dient zum Betrieb   */
/*  des ON-Chip A/D-Wandlers des AT89C51CC03. Der Funktion wird der Eingangs-   */
/*  kanal �bergeben und sie liefert ein 10-Bit Wandlungsergebnis als Funktions- */
/*  wert zur�ck.                                                                */
/*  Wichtig:        Diese Funktion setzt voraus, das die Grundinitialisierung   */
/*                  des ON-Chip A/D-Wandlers ausserhalb der Funktion, also      */
/*                  z. B. im Hauptprogramm erfolgt. Zur Grundinitialisierung    */
/*                  muss der A/D-Wandler eingeschaltet werden, also das         */
/*                  ADEN-Bit im ADCON SFR gesetz werden ( ADCON = 0x20) und     */
/*                  der zu wandelnde Eingangskanal im SFR ADCF als Analog-      */
/*                  eingang definiert werden (z. B. ADCF = 0xff ->              */
/*                  gesamter Port 1 als Analogeingang definiert!                */
/*  Parameter kanal := Auswahl des A/D-Wandler Eingangskanals (0..7)            */
/********************************************************************************/

unsigned int mw_erf10(unsigned char kanal)
  {      
    unsigned int erg;                       // Variable f�r 10-Bit Wandlungsergebnis
    ADCON = (ADCON & 0xe8);                 // Kanalauswahl und Wandlungsende-Bit r�cksetzen!
    ADCON = (ADCON | (0x08+kanal));         // Kanalauswahl und Start der einmaligen Wandlung!
    while((ADCON & 0x10) == 0);             // Warte bis A/D-Wandlung beendet ist!
    erg = ((ADDH << 2) + (ADDL & 0x03));    // Ergebnisregister auslesen u. zusammenbauen!
    return(erg);
  }
