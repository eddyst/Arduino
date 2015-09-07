typedef struct {
//  uint8_t Changed  ;  int16_t ValueX10 ;
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
  prog_char *Name;
} data;
#define _HKRuecklaufTemp             0
#define _HKRuecklaufTempMieter       1
#define _HKRuecklaufTempSteier       2
#define _HKVorlaufTemp               3
#define _HKVorlaufTempGemischt       4
#define _HKVorlaufValue              5

#define _KollektorAusdehnung         6
#define _KollektorWTRuecklauf        7
#define _KollektorWTVorlauf          8        

#define _SolarRuecklauf              9
#define _SolarVorlauf               10
#define _SolarWTRuecklauf           11
#define _SolarWTVorlauf             12   

#define _SpeicherA1                 13
#define _SpeicherA2                 14
#define _SpeicherA3                 15
#define _SpeicherA4                 16
#define _SpeicherA5                 17

#define _ThermeAussenTemp           18
#define _ThermeBetriebsart          19
#define _ThermeBrennerLeistung      20
#define _ThermeKesselTempIst        21
#define _ThermeKesselTempSoll       22
#define _ThermeRuecklaufTemp        23
#define _ThermeRuecklaufTempOpto    24
#define _ThermeSpeicherTemp         25
#define _ThermeUmschaltventilOpto   26
#define _ThermeUmschaltventilTaster 27
#define _ThermeVorlaufTemp          28
#define _ThermeVorlaufTempSoll      29

#define _WWPumpeProzent             30
#define _WWRuecklaufTemp            31
#define _WWSpeicherTemp1            32
#define _WWSpeicherTemp2            33
#define _WWVorlaufTemp              34

uint8_t  owArray[] = { _HKRuecklaufTemp, _HKRuecklaufTempMieter, _HKRuecklaufTempSteier, _HKVorlaufTemp, _HKVorlaufTempGemischt,
                       _KollektorAusdehnung,  _KollektorWTRuecklauf,_KollektorWTVorlauf,
                       _SolarRuecklauf, _SolarVorlauf, _SolarWTRuecklauf, _SolarWTVorlauf,
                       _SpeicherA1, _SpeicherA2, _SpeicherA3, _SpeicherA4, _SpeicherA5,
                       _ThermeRuecklaufTemp, _ThermeVorlaufTemp,
                       _WWPumpeProzent, _WWRuecklaufTemp, _WWSpeicherTemp1, _WWSpeicherTemp2, _WWVorlaufTemp };

prog_char FHEM_HKRuecklaufTemp            [] myPROGMEM =  "HKRuecklaufTemp";
prog_char FHEM_HKRuecklaufTempMieter      [] myPROGMEM =  "HKRuecklaufTempMieter";
prog_char FHEM_HKRuecklaufTempSteier      [] myPROGMEM =  "HKRuecklaufTempSteier";
prog_char FHEM_HKVorlaufTemp              [] myPROGMEM =  "HKVorlaufTemp";
prog_char FHEM_HKVorlaufTempGemischt      [] myPROGMEM =  "HKVorlaufTempGemischt";
prog_char FHEM_HKVorlaufValue             [] myPROGMEM =  "HKVorlaufValue";

prog_char FHEM_KollektorAusdehnung        [] myPROGMEM =  "KollektorAusdehnung";
prog_char FHEM_KollektorWTRuecklauf       [] myPROGMEM =  "KollektorWTRuecklauf";
prog_char FHEM_KollektorWTVorlauf         [] myPROGMEM =  "KollektorWTVorlauf";

prog_char FHEM_SolarRuecklauf             [] myPROGMEM =  "SolarRuecklauf";
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

prog_char FHEM_WWPumpeProzent             [] myPROGMEM =  "WWPumpeProzent";
prog_char FHEM_WWRuecklaufTemp            [] myPROGMEM =  "WWRuecklaufTemp";
prog_char FHEM_WWSpeicherTemp1            [] myPROGMEM =  "WWSpeicherTemp1";
prog_char FHEM_WWSpeicherTemp2            [] myPROGMEM =  "WWSpeicherTemp2";
prog_char FHEM_WWVorlaufTemp              [] myPROGMEM =  "WWVorlaufTemp";


#define ValueUnknown -10000
data Values[] = {{ 0, ValueUnknown, FHEM_HKRuecklaufTemp            }, // 0 
                 { 0, ValueUnknown, FHEM_HKRuecklaufTempMieter      }, 
                 { 0, ValueUnknown, FHEM_HKRuecklaufTempSteier      }, 
                 { 0, ValueUnknown, FHEM_HKVorlaufTemp              }, 
                 { 0, ValueUnknown, FHEM_HKVorlaufTempGemischt      }, 
                 { 0, ValueUnknown, FHEM_HKVorlaufValue             }, // 5

                 { 0, ValueUnknown, FHEM_KollektorAusdehnung        }, 
                 { 0, ValueUnknown, FHEM_KollektorWTRuecklauf       }, 
                 { 0, ValueUnknown, FHEM_KollektorWTVorlauf         }, 

                 { 0, ValueUnknown, FHEM_SolarRuecklauf             }, 
                 { 0, ValueUnknown, FHEM_SolarVorlauf               }, //10
                 { 0, ValueUnknown, FHEM_SolarWTRuecklauf           }, 
                 { 0, ValueUnknown, FHEM_SolarWTVorlauf             }, 

                 { 0, ValueUnknown, FHEM_SpeicherA1                 }, 
                 { 0, ValueUnknown, FHEM_SpeicherA2                 }, 
                 { 0, ValueUnknown, FHEM_SpeicherA3                 }, //15
                 { 0, ValueUnknown, FHEM_SpeicherA4                 }, 
                 { 0, ValueUnknown, FHEM_SpeicherA5                 }, 

                 { 0, ValueUnknown, FHEM_ThermeAussenTemp           }, 
                 { 0, ValueUnknown, FHEM_ThermeBetriebsart          }, 
                 { 0, ValueUnknown, FHEM_ThermeBrennerLeistung      }, //20
                 { 0, ValueUnknown, FHEM_ThermeKesselTempIst        }, 
                 { 0, ValueUnknown, FHEM_ThermeKesselTempSoll       },
                 { 0, ValueUnknown, FHEM_ThermeRuecklaufTemp        }, 
                 { 0, ValueUnknown, FHEM_ThermeRuecklaufTempOpto    }, 
                 { 0, ValueUnknown, FHEM_ThermeSpeicherTemp         }, //25
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilOpto   }, 
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilTaster }, 
                 { 0, ValueUnknown, FHEM_ThermeVorlaufTemp          }, 
                 { 0, ValueUnknown, FHEM_ThermeVorlaufTempSoll      }, 
                 
                 { 0, ValueUnknown, FHEM_WWPumpeProzent             }, //30 
                 { 0, ValueUnknown, FHEM_WWRuecklaufTemp            },
                 { 0, ValueUnknown, FHEM_WWSpeicherTemp1            }, 
                 { 0, ValueUnknown, FHEM_WWSpeicherTemp2            }, 
                 { 0, ValueUnknown, FHEM_WWVorlaufTemp              }
                };

boolean  Solarbetrieb          = false;
uint8_t  HKSollTempVorgabe     = 40; //ToDo: Abfragen
#define  HKHysterese              5
uint8_t  WWSollTempVorgabe     = 45; //ToDo: Abfragen
#define  WWHysterese              5
         #define ThermeBetriebsartAbschalten       0
         #define ThermeBetriebsartWW               1
         #define ThermeBetriebsartHeizenUndWW      2
         #define ThermeBetriebsartDauerndReduziert 3
         #define ThermeBetriebsartDauerndNormal    4

char buffer[30]; //Allgemeiner Buffer 


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

