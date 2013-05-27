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
#define _HKVorlaufGemischt          11
#define _HKRuecklauf                12
#define _HKRuecklaufMieter          13
#define _HKRuecklaufSteier          14

prog_char FHEM_ThermeKesselTempSoll       [] PROGMEM =  "ThermeKesselTempSoll";
prog_char FHEM_ThermeKesselTempIst        [] PROGMEM =  "ThermeKesselTempIst";
prog_char FHEM_ThermeVorlaufTempSoll      [] PROGMEM =  "ThermeVorlaufTempSoll";
prog_char FHEM_ThermeRuecklaufTemp        [] PROGMEM =  "ThermeRuecklaufTemp";
prog_char FHEM_ThermeSpeicherTemp         [] PROGMEM =  "ThermeSpeicherTemp";
prog_char FHEM_ThermeAussenTemp           [] PROGMEM =  "ThermeAussenTemp";
prog_char FHEM_ThermeBetriebsart          [] PROGMEM =  "ThermeBetriebsart";
prog_char FHEM_ThermeUmschaltventilOpto   [] PROGMEM =  "ThermeUmschaltventilOpto";
prog_char FHEM_ThermeUmschaltventilTaster [] PROGMEM =  "ThermeUmschaltventilTaster";
prog_char FHEM_ThermeBrennerLeistung      [] PROGMEM =  "ThermeBrennerLeistung";
prog_char FHEM_HKVorlauf                  [] PROGMEM =  "HKVorlauf";
prog_char FHEM_HKVorlaufGemischt          [] PROGMEM =  "HKVorlaufGemischt";
prog_char FHEM_HKRuecklauf                [] PROGMEM =  "HKRuecklauf";
prog_char FHEM_HKRuecklaufMieter          [] PROGMEM =  "HKRuecklaufMieter";
prog_char FHEM_HKRuecklaufSteier          [] PROGMEM =  "HKRuecklaufSteier";

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
                 { 0, ValueUnknown, FHEM_HKVorlaufGemischt          }, 
                 { 0, ValueUnknown, FHEM_HKRuecklauf                }, 
                 { 0, ValueUnknown, FHEM_HKRuecklaufMieter          }, 
                 { 0, ValueUnknown, FHEM_HKRuecklaufSteier          }  
                };

uint8_t  owAddr[][9] = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 10},
                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 11},
                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 12},
                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 13},
                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 14} };

uint8_t  ThermeVorlaufTempVorgabe = 50; //ToDo: Abfragen und bei Änderung KesselSollTempVorgabeRechnen();
uint8_t  HKSollTempVorgabe     = 45; //ToDo: Abfragen

         #define ThermeBetriebsartAbschalten       0
         #define ThermeBetriebsartWW               1
         #define ThermeBetriebsartHeizenUndWW      2
         #define ThermeBetriebsartDauerndReduziert 3
         #define ThermeBetriebsartDauerndNormal    4

uint8_t Menu = 0;
uint8_t EthState = 0;
/*
String toHex (uint8_t value) {
  if ( value < 16)
    return String("0") + String(value, HEX);
  else
    return String(value, HEX);
}
*/


