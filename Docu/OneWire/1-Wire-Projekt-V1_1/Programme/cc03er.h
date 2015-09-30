/********************************************************************************/
/*                                                                              */
/* DATEI: cc03er.h   Autoren: B.v.B. / P.G.                                     */
/*                                                                              */
/* Header-Datei mit den externen Funktionsprototypen Deklarationen zur          */
/* C-Source-Code Datei cc03er.c!                                                */
/*                                                                              */
/* Letzte Änderung: 20.12.2008                                                  */
/*                                                                              */
/********************************************************************************/

#ifndef  _CC03ER_H_
  
  #define  _CC03ER_H_

    extern void seri_init(void);  
    extern unsigned char seri_zeichen_da(void);
    extern unsigned char seri_empf(void);
    extern seri_send(unsigned char DATEN);
    extern unsigned char mw_erf8(unsigned char);         
    extern unsigned int mw_erf10(unsigned char);         

#endif

