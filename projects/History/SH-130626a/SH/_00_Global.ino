#define _HKAnforderung               0
#define _HKRuecklaufTemp2            1
#define _HKRuecklaufTemp1Mieter      2
#define _HKRuecklaufTemp1Steier      3
#define _HKVorlaufTemp1              4
#define _HKVorlaufTemp2              5
#define _HKVorlaufValue              6

#define _KollektorAusdehnung         7
#define _KollektorStatus             8
#define _KollektorWTRuecklauf        9
#define _KollektorWTVorlauf         10        

#define _SolarRuecklauf             11
#define _SolarStatus                12
#define _SolarVorlauf               13
#define _SolarWTRuecklauf           14
#define _SolarWTVorlauf             15   

#define _SpeicherA1                 16
#define _SpeicherA2                 17
#define _SpeicherA3                 18
#define _SpeicherA4                 19
#define _SpeicherA5                 20

#define _ThermeAussenTemp           21
#define _ThermeBetriebsart          22
#define _ThermeBrennerLeistung      23
#define _ThermeKesselTempIst        24
#define _ThermeKesselTempSoll       25
#define _ThermeRuecklaufTemp        26
#define _ThermeRuecklaufTempOpto    27
#define _ThermeSpeicherTemp         28
#define _ThermeUmschaltventilOpto   29
#define _ThermeUmschaltventilTaster 30
#define _ThermeVorlaufTemp          31
#define _ThermeVorlaufTempSoll      32

#define _WWAnforderung              33
#define _WWPumpeProzent             34
#define _WWRuecklaufTemp            35
#define _WWSpeicherTemp1            36
#define _WWSpeicherTemp2            37
#define _WWVorlaufTemp              38

//Das folgende Arraykönnen wir durchlaufen. Der Index ist die Reihenfolge der OWAddr im EEProm, der Wert entspricht der Indexnummer im Values-Array 
#define ValueUnknown -10000
typedef struct {
  uint8_t ValueIndex;
   int8_t Last2;
   int8_t Last1;
} t_owArray;
t_owArray  owArray[] = {{ _HKRuecklaufTemp2, ValueUnknown, ValueUnknown}, 
                        { _HKRuecklaufTemp1Mieter, ValueUnknown, ValueUnknown}, 
                        { _HKRuecklaufTemp1Steier, ValueUnknown, ValueUnknown}, 
                        { _HKVorlaufTemp1, ValueUnknown, ValueUnknown}, 
                        { _HKVorlaufTemp2, ValueUnknown, ValueUnknown},
                        
                        { _KollektorAusdehnung, ValueUnknown, ValueUnknown}, 
                        { _KollektorWTRuecklauf, ValueUnknown, ValueUnknown}, 
                        { _KollektorWTVorlauf, ValueUnknown, ValueUnknown},
                        
                        { _SolarRuecklauf, ValueUnknown, ValueUnknown}, 
                        { _SolarVorlauf, ValueUnknown, ValueUnknown}, 
                        { _SolarWTRuecklauf, ValueUnknown, ValueUnknown}, 
                        { _SolarWTVorlauf, ValueUnknown, ValueUnknown},
                        
                        { _SpeicherA1, ValueUnknown, ValueUnknown}, 
                        { _SpeicherA2, ValueUnknown, ValueUnknown}, 
                        { _SpeicherA3, ValueUnknown, ValueUnknown}, 
                        { _SpeicherA4, ValueUnknown, ValueUnknown}, 
                        { _SpeicherA5, ValueUnknown, ValueUnknown},
                        
                        { _ThermeRuecklaufTemp, ValueUnknown, ValueUnknown}, 
                        { _ThermeVorlaufTemp, ValueUnknown, ValueUnknown},
                        
                        { _WWPumpeProzent, ValueUnknown, ValueUnknown}, 
                        { _WWRuecklaufTemp, ValueUnknown, ValueUnknown}, 
                        { _WWSpeicherTemp1, ValueUnknown, ValueUnknown}, 
                        { _WWSpeicherTemp2, ValueUnknown, ValueUnknown}, 
                        { _WWVorlaufTemp, ValueUnknown, ValueUnknown} 
                       };

prog_char FHEM_HKAnforderung              [] myPROGMEM =  "HKAnforderung";
prog_char FHEM_HKRuecklaufTemp2           [] myPROGMEM =  "HKRuecklaufTemp2";
prog_char FHEM_HKRuecklaufTemp1Mieter     [] myPROGMEM =  "HKRuecklaufTemp1Mieter";
prog_char FHEM_HKRuecklaufTemp1Steier     [] myPROGMEM =  "HKRuecklaufTemp1Steier";
prog_char FHEM_HKVorlaufTemp1             [] myPROGMEM =  "HKVorlaufTemp1";
prog_char FHEM_HKVorlaufTemp2             [] myPROGMEM =  "HKVorlaufTemp2";
prog_char FHEM_HKVorlaufValue             [] myPROGMEM =  "HKVorlaufValue";

prog_char FHEM_KollektorAusdehnung        [] myPROGMEM =  "KollektorAusdehnung";
prog_char FHEM_KollektorStatus            [] myPROGMEM =  "KollektorStatus";
prog_char FHEM_KollektorWTRuecklauf       [] myPROGMEM =  "KollektorWTRuecklauf";
prog_char FHEM_KollektorWTVorlauf         [] myPROGMEM =  "KollektorWTVorlauf";

prog_char FHEM_SolarRuecklauf             [] myPROGMEM =  "SolarRuecklauf";
prog_char FHEM_SolarStatus                [] myPROGMEM =  "SolarStatus";
prog_char FHEM_SolarVorlauf               [] myPROGMEM =  "SolarVorlauf";
prog_char FHEM_SolarWTRuecklauf           [] myPROGMEM =  "SolarWTRuecklauf";
prog_char FHEM_SolarWTVorlauf             [] myPROGMEM =  "SolarWTVorlauf";

prog_char FHEM_SpeicherA1                 [] myPROGMEM =  "SpeicherA1";
prog_char FHEM_SpeicherA2                 [] myPROGMEM =  "SpeicherA2";
prog_char FHEM_SpeicherA3                 [] myPROGMEM =  "SpeicherA3";
prog_char FHEM_SpeicherA4                 [] myPROGMEM =  "SpeicherA4";
prog_char FHEM_SpeicherA5                 [] myPROGMEM =  "SpeicherA5";

prog_char FHEM_ThermeAussenTemp           [] myPROGMEM =  "ThermeAussenTemp";
prog_char FHEM_ThermeBetriebsart          [] myPROGMEM =  "ThermeBetriebsart";
prog_char FHEM_ThermeBrennerLeistung      [] myPROGMEM =  "ThermeBrennerLeistung";
prog_char FHEM_ThermeKesselTempIst        [] myPROGMEM =  "ThermeKesselTempIst";
prog_char FHEM_ThermeKesselTempSoll       [] myPROGMEM =  "ThermeKesselTempSoll";
prog_char FHEM_ThermeRuecklaufTemp        [] myPROGMEM =  "ThermeRuecklaufTemp";
prog_char FHEM_ThermeRuecklaufTempOpto    [] myPROGMEM =  "ThermeRuecklaufTempOpto";
prog_char FHEM_ThermeSpeicherTemp         [] myPROGMEM =  "ThermeSpeicherTemp";
prog_char FHEM_ThermeUmschaltventilOpto   [] myPROGMEM =  "ThermeUmschaltventilOpto";
prog_char FHEM_ThermeUmschaltventilTaster [] myPROGMEM =  "ThermeUmschaltventilTaster";
prog_char FHEM_ThermeVorlaufTemp          [] myPROGMEM =  "ThermeVorlaufTemp";
prog_char FHEM_ThermeVorlaufTempSoll      [] myPROGMEM =  "ThermeVorlaufTempSoll";

prog_char FHEM_WWAnforderung              [] myPROGMEM =  "WWAnforderung";
prog_char FHEM_WWPumpeProzent             [] myPROGMEM =  "WWPumpeProzent";
prog_char FHEM_WWRuecklaufTemp            [] myPROGMEM =  "WWRuecklaufTemp";
prog_char FHEM_WWSpeicherTemp1            [] myPROGMEM =  "WWSpeicherTemp1";
prog_char FHEM_WWSpeicherTemp2            [] myPROGMEM =  "WWSpeicherTemp2";
prog_char FHEM_WWVorlaufTemp              [] myPROGMEM =  "WWVorlaufTemp";


#define ValueUnknown -10000
typedef struct {
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
  prog_char *Name;
} t_Values;
t_Values Values[] = {{ 0, ValueUnknown, FHEM_HKAnforderung              }, // 0
                     { 0, ValueUnknown, FHEM_HKRuecklaufTemp2           },  
                     { 0, ValueUnknown, FHEM_HKRuecklaufTemp1Mieter     }, 
                     { 0, ValueUnknown, FHEM_HKRuecklaufTemp1Steier     }, 
                     { 0, ValueUnknown, FHEM_HKVorlaufTemp1             }, 
                     { 0, ValueUnknown, FHEM_HKVorlaufTemp2             }, // 5
                     { 0, ValueUnknown, FHEM_HKVorlaufValue             }, 

                     { 0, ValueUnknown, FHEM_KollektorAusdehnung        }, 
                     { 0, ValueUnknown, FHEM_KollektorStatus            }, 
                     { 0, ValueUnknown, FHEM_KollektorWTRuecklauf       }, 
                     { 0, ValueUnknown, FHEM_KollektorWTVorlauf         }, //10

                     { 0, ValueUnknown, FHEM_SolarRuecklauf             }, 
                     { 0, ValueUnknown, FHEM_SolarStatus                }, 
                     { 0, ValueUnknown, FHEM_SolarVorlauf               }, 
                     { 0, ValueUnknown, FHEM_SolarWTRuecklauf           }, 
                     { 0, ValueUnknown, FHEM_SolarWTVorlauf             }, //15

                     { 0, ValueUnknown, FHEM_SpeicherA1                 }, 
                     { 0, ValueUnknown, FHEM_SpeicherA2                 }, 
                     { 0, ValueUnknown, FHEM_SpeicherA3                 }, 
                     { 0, ValueUnknown, FHEM_SpeicherA4                 }, 
                     { 0, ValueUnknown, FHEM_SpeicherA5                 }, //20

                     { 0, ValueUnknown, FHEM_ThermeAussenTemp           }, 
                     { 0, ValueUnknown, FHEM_ThermeBetriebsart          }, 
                     { 0, ValueUnknown, FHEM_ThermeBrennerLeistung      }, 
                     { 0, ValueUnknown, FHEM_ThermeKesselTempIst        }, 
                     { 0, ValueUnknown, FHEM_ThermeKesselTempSoll       }, //25
                     { 0, ValueUnknown, FHEM_ThermeRuecklaufTemp        }, 
                     { 0, ValueUnknown, FHEM_ThermeRuecklaufTempOpto    }, 
                     { 0, ValueUnknown, FHEM_ThermeSpeicherTemp         }, 
                     { 0, ValueUnknown, FHEM_ThermeUmschaltventilOpto   }, 
                     { 0, ValueUnknown, FHEM_ThermeUmschaltventilTaster }, //30
                     { 0, ValueUnknown, FHEM_ThermeVorlaufTemp          }, 
                     { 0, ValueUnknown, FHEM_ThermeVorlaufTempSoll      },  
                 
                     { 0, ValueUnknown, FHEM_WWAnforderung              }, 
                     { 0, ValueUnknown, FHEM_WWPumpeProzent             }, 
                     { 0, ValueUnknown, FHEM_WWRuecklaufTemp            }, //35
                     { 0, ValueUnknown, FHEM_WWSpeicherTemp1            }, 
                     { 0, ValueUnknown, FHEM_WWSpeicherTemp2            },  
                     { 0, ValueUnknown, FHEM_WWVorlaufTemp              }
                    };

boolean  Solarbetrieb          = false;
uint8_t  HKSollTempVorgabe     = 30; //ToDo: Abfragen
#define  HKHysterese              5
uint8_t  WWSollTempVorgabe     = 45; //ToDo: Abfragen
#define  WWHysterese              5
         #define ThermeBetriebsartAbschalten       0
         #define ThermeBetriebsartWW               1
         #define ThermeBetriebsartHeizenUndWW      2
         #define ThermeBetriebsartDauerndReduziert 3
         #define ThermeBetriebsartDauerndNormal    4

char buffer[30]; //Allgemeiner Buffer 
#define EEPROM_Offset_Stagnation   0
#define EEPROM_Offset_owArray    100

// Converts an ASCII character to a numeric value.
// Returns true if the character is numeric, returns
// false for all other characters.
// Utilises the fact that char type is just a byte.
// Thus '7' (ASCII 55) minus '0' (ASCII 48) equals 7.
boolean parseHexChar(const char& input, uint8_t& output) {
  if(input >= '0' && input <= '9') {
    output = input - '0';
    return true; 
  } else if(input >= 'A' && input <= 'F'){
    output = input - 'A' + 10;
    return true; 
  } else if(input >= 'a' && input <= 'f'){
    output = input - 'a' + 10;
    return true; 
  }
  output = 0;
  return false;
}

