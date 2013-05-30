typedef struct {
//  uint8_t Changed  ;  int16_t ValueX10 ;
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
  prog_char *Name;
} data;
#define _ThermeKesselTempSoll        0
#define _ThermeKesselTempIst         1
#define _ThermeVorlaufTempSoll       2
#define _ThermeRuecklaufTemp         3
#define _ThermeSpeicherTemp          4
#define _ThermeAussenTemp            5
#define _ThermeBetriebsart           6
#define _ThermeUmschaltventilOpto    7
#define _ThermeUmschaltventilTaster  8
#define _ThermeBrennerLeistung       9
#define _HKVorlauf                  10
#define _HKVorlaufValue             11
#define _HKVorlaufGemischt          12
#define _HKRuecklauf                13
#define _HKRuecklaufMieter          14
#define _HKRuecklaufSteier          15

prog_char FHEM_ThermeKesselTempSoll       [] myPROGMEM =  "ThermeKesselTempSoll";
prog_char FHEM_ThermeKesselTempIst        [] myPROGMEM =  "ThermeKesselTempIst";
prog_char FHEM_ThermeVorlaufTempSoll      [] myPROGMEM =  "ThermeVorlaufTempSoll";
prog_char FHEM_ThermeRuecklaufTemp        [] myPROGMEM =  "ThermeRuecklaufTemp";
prog_char FHEM_ThermeSpeicherTemp         [] myPROGMEM =  "ThermeSpeicherTemp";
prog_char FHEM_ThermeAussenTemp           [] myPROGMEM =  "ThermeAussenTemp";
prog_char FHEM_ThermeBetriebsart          [] myPROGMEM =  "ThermeBetriebsart";
prog_char FHEM_ThermeUmschaltventilOpto   [] myPROGMEM =  "ThermeUmschaltventilOpto";
prog_char FHEM_ThermeUmschaltventilTaster [] myPROGMEM =  "ThermeUmschaltventilTaster";
prog_char FHEM_ThermeBrennerLeistung      [] myPROGMEM =  "ThermeBrennerLeistung";
prog_char FHEM_HKVorlauf                  [] myPROGMEM =  "HKVorlauf";
prog_char FHEM_HKVorlaufValue             [] myPROGMEM =  "HKVorlaufValue";
prog_char FHEM_HKVorlaufGemischt          [] myPROGMEM =  "HKVorlaufGemischt";
prog_char FHEM_HKRuecklauf                [] myPROGMEM =  "HKRuecklauf";
prog_char FHEM_HKRuecklaufMieter          [] myPROGMEM =  "HKRuecklaufMieter";
prog_char FHEM_HKRuecklaufSteier          [] myPROGMEM =  "HKRuecklaufSteier";

uint8_t  owArray[] = { _HKVorlauf, _HKVorlaufGemischt, _HKRuecklauf, _HKRuecklaufMieter, _HKRuecklaufSteier };

#define ValueUnknown -10000
data Values[] = {{ 0, ValueUnknown, FHEM_ThermeKesselTempSoll       }, // 0
                 { 0, ValueUnknown, FHEM_ThermeKesselTempIst        }, 
                 { 0, ValueUnknown, FHEM_ThermeVorlaufTempSoll      }, 
                 { 0, ValueUnknown, FHEM_ThermeRuecklaufTemp        }, 
                 { 0, ValueUnknown, FHEM_ThermeSpeicherTemp         }, 
                 { 0, ValueUnknown, FHEM_ThermeAussenTemp           }, // 5
                 { 0, ValueUnknown, FHEM_ThermeBetriebsart          }, 
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilOpto   }, 
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilTaster }, 
                 { 0, ValueUnknown, FHEM_ThermeBrennerLeistung      }, 
                 { 0, ValueUnknown, FHEM_HKVorlauf                  }, //10
                 { 0, ValueUnknown, FHEM_HKVorlaufValue             }, 
                 { 0, ValueUnknown, FHEM_HKVorlaufGemischt          }, 
                 { 0, ValueUnknown, FHEM_HKRuecklauf                }, 
                 { 0, ValueUnknown, FHEM_HKRuecklaufMieter          }, 
                 { 0, ValueUnknown, FHEM_HKRuecklaufSteier          }  
                };

uint8_t  ThermeVorlaufTempVorgabe = 50; //ToDo: Abfragen und bei Änderung KesselSollTempVorgabeRechnen();
uint8_t  HKSollTempVorgabe     = 45; //ToDo: Abfragen

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

