/********************************************************************************************/
/*                                                                                          */
/*     TFH Georg Agricola, Bochum   /   Elektor-Verlag, Aachen                              */
/*                                                                                          */
/*     Fernlehrgang-Funktionssammlung 2, Betrieb des Terminals:  Datei terminal.c           */
/*                                                                                          */
/*     Funktionen:                                                                          */
/*     clear_screen :=  Löscht den Terminal Bildschirm und setz Cursor auf Home-Position!   */
/*     set_cursor   :=  Positioniert den Cursor auf die angegebene Zeile und Spalte!        */
/*                                                                                          */
/*     Autoren:           P.G. / B.v.B.                                                     */
/*                                                                                          */
/*     Zielsystem:        TFH-Core ONE, Prozessor: AT89C51CC03 (Atmel)                      */
/*                                                                                          */
/*                                                                                          */
/*     IDE/C-Compiler:    uC/51 (Wickenhäuser)                                              */
/*                                                                                          */
/*     Letzte Änderung:   21.09.2008                                                        */
/*                                                                                          */
/*     Datei:             terminal.c                                                        */
/*                                                                                          */
/********************************************************************************************/

/*** Einbinden von Include-Dateien ***/
#include<stdio.h>

/********************************************************************************/
/*  Funktion void clear_screen(void) löscht den Terminal Bildschirm!            */
/********************************************************************************/

void clear_screen(void)
  {
    printf("\x1b\x48\x1b\x4a");
  }

/********************************************************************************/
/*  Funktion void set_cursor(unsigned char z, unsigned char s) positioniert den */
/*  Terminal Cursor auf die angegebene Position:                                */
/*  Parameter z := Zeile, Parameter s := Spalte                                 */
/********************************************************************************/

void set_cursor(unsigned char z, unsigned char s)
  {
    printf("\x1b\x59%c%c",(32+z),(32+s));
  }  

