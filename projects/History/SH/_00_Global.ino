typedef struct {
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
//  prog_char *Name;
} data;

#define       _HKAnforderung               0
//prog_char FHEM_HKAnforderung              [] PROGMEM =  "HKAnforderung";
//#define       _HKRuecklaufTemp1Mieter      ?
//prog_char FHEM_HKRuecklaufTemp1Mieter     [] PROGMEM =  "HKRuecklaufTemp1Mieter";
//#define       _HKRuecklaufTemp1Steier      ?
//prog_char FHEM_HKRuecklaufTemp1Steier     [] PROGMEM =  "HKRuecklaufTemp1Steier";
#define       _HKRuecklaufTemp2            1
//prog_char FHEM_HKRuecklaufTemp2           [] PROGMEM =  "HKRuecklaufTemp2";
#define       _HKVorlaufTemp1              2
//prog_char FHEM_HKVorlaufTemp1             [] PROGMEM =  "HKVorlaufTemp1";
#define       _HKVorlaufTemp2              3
//prog_char FHEM_HKVorlaufTemp2             [] PROGMEM =  "HKVorlaufTemp2";
#define       _HKVorlaufTempSoll           4
//prog_char FHEM_HKVorlaufTempSoll          [] PROGMEM =  "HKVorlaufTempSoll";
#define       _HKVorlaufValue              5
//prog_char FHEM_HKVorlaufValue             [] PROGMEM =  "HKVorlaufValue";
#define       _UNUSED              44
//prog_char FHEM_UNUSED             [] PROGMEM =  "UNUSED";

#define       _KollektorAusdehnung         6
//prog_char FHEM_KollektorAusdehnung        [] PROGMEM =  "KollektorAusdehnung";
#define       _KollektorStatus             7
//prog_char FHEM_KollektorStatus            [] PROGMEM =  "KollektorStatus";
#define       _KollektorWTRuecklauf        8
//prog_char FHEM_KollektorWTRuecklauf       [] PROGMEM =  "KollektorWTRuecklauf";
#define       _KollektorWTVorlauf          9        
//prog_char FHEM_KollektorWTVorlauf         [] PROGMEM =  "KollektorWTVorlauf";

#define       _SolarRuecklauf             10
//prog_char FHEM_SolarRuecklauf             [] PROGMEM =  "SolarRuecklauf";
#define       _SolarStatus                11
//prog_char FHEM_SolarStatus                [] PROGMEM =  "SolarStatus";
#define       _SolarVorlauf               12
//prog_char FHEM_SolarVorlauf               [] PROGMEM =  "SolarVorlauf";
#define       _SolarWTRuecklauf           13
//prog_char FHEM_SolarWTRuecklauf           [] PROGMEM =  "SolarWTRuecklauf";
#define       _SolarWTVorlauf             14   
//prog_char FHEM_SolarWTVorlauf             [] PROGMEM =  "SolarWTVorlauf";

#define       _SpeicherA1                 15
//prog_char FHEM_SpeicherA1                 [] PROGMEM =  "SpeicherA1";
#define       _SpeicherA2                 16
//prog_char FHEM_SpeicherA2                 [] PROGMEM =  "SpeicherA2";
#define       _SpeicherA3                 17
//prog_char FHEM_SpeicherA3                 [] PROGMEM =  "SpeicherA3";
#define       _SpeicherA4                 18
//prog_char FHEM_SpeicherA4                 [] PROGMEM =  "SpeicherA4";
#define       _SpeicherA5                 19
//prog_char FHEM_SpeicherA5                 [] PROGMEM =  "SpeicherA5";

#define       _SteuerungMemFree           20
//prog_char FHEM_SteuerungMemFree           [] PROGMEM =  "SteuerungMemFree";
#define       _SteuerungStatus            21
//prog_char FHEM_SteuerungStatus            [] PROGMEM =  "SteuerungStatus";

#define       _ThermeAussenTemp           22
//prog_char FHEM_ThermeAussenTemp           [] PROGMEM =  "ThermeAussenTemp";
#define       _ThermeBetriebsart          23
//prog_char FHEM_ThermeBetriebsart          [] PROGMEM =  "ThermeBetriebsart";
#define       _ThermeBrennerLeistung      24
//prog_char FHEM_ThermeBrennerLeistung      [] PROGMEM =  "ThermeBrennerLeistung";
#define       _ThermeKesselTempIst        25
//prog_char FHEM_ThermeKesselTempIst        [] PROGMEM =  "ThermeKesselTempIst";
#define       _ThermeKesselTempSoll       26
//prog_char FHEM_ThermeKesselTempSoll       [] PROGMEM =  "ThermeKesselTempSoll";
#define       _ThermeRuecklaufTempIst     27
//prog_char FHEM_ThermeRuecklaufTempIst     [] PROGMEM =  "ThermeRuecklaufTempIst";
#define       _ThermeRuecklaufTempOpto    28
//prog_char FHEM_ThermeRuecklaufTempOpto    [] PROGMEM =  "ThermeRuecklaufTempOpto";
#define       _ThermeSpeicherTemp         29
//prog_char FHEM_ThermeSpeicherTemp         [] PROGMEM =  "ThermeSpeicherTemp";
#define       _ThermeUmschaltventilOpto   30
//prog_char FHEM_ThermeUmschaltventilOpto   [] PROGMEM =  "ThermeUmschaltventilOpto";
#define       _ThermeUmschaltventilTaster 31
//prog_char FHEM_ThermeUmschaltventilTaster [] PROGMEM =  "ThermeUmschaltventilTaster";
#define       _ThermeVorlaufTempIst       32
//prog_char FHEM_ThermeVorlaufTempIst       [] PROGMEM =  "ThermeVorlaufTempIst";
#define       _ThermeVorlaufTempSoll      33
//prog_char FHEM_ThermeVorlaufTempSoll      [] PROGMEM =  "ThermeVorlaufTempSoll";
#define       _ThermeVorlaufValue         34
//prog_char FHEM_ThermeVorlaufValue         [] PROGMEM =  "ThermeVorlaufValue";
#define       _ThermeVorlaufVentil        35
//prog_char FHEM_ThermeVorlaufVentil        [] PROGMEM =  "ThermeVorlaufVentil";

#define       _WWAnforderung              36
//prog_char FHEM_WWAnforderung              [] PROGMEM =  "WWAnforderung";
#define       _WWPumpeProzent             37
//prog_char FHEM_WWPumpeProzent             [] PROGMEM =  "WWPumpeProzent";
#define       _WWRuecklaufTemp            38
//prog_char FHEM_WWRuecklaufTemp            [] PROGMEM =  "WWRuecklaufTemp";
#define       _WWSpeicherTemp1            39
//prog_char FHEM_WWSpeicherTemp1            [] PROGMEM =  "WWSpeicherTemp1";
#define       _WWSpeicherTemp2            40
//prog_char FHEM_WWSpeicherTemp2            [] PROGMEM =  "WWSpeicherTemp2";
#define       _WWSpeicherTempSoll         41
//prog_char FHEM_WWSpeicherTempSoll         [] PROGMEM =  "WWSpeicherTempSoll";
#define       _WWVentil                   42
//prog_char FHEM_WWVentil                   [] PROGMEM =  "WWVentil";
#define       _WWVorlaufTemp              43
//prog_char FHEM_WWVorlaufTemp              [] PROGMEM =  "WWVorlaufTemp";
#define       _WWZirkulation              45
//prog_char FHEM_WWZirkulation              [] PROGMEM =  "WWZirkulation";

data Values[46];

uint8_t  owArray[] = { _HKRuecklaufTemp2, _HKVorlaufTemp1, _HKVorlaufTemp2,
                       _KollektorAusdehnung,  _KollektorWTRuecklauf,_KollektorWTVorlauf,
                       _SolarRuecklauf, _SolarVorlauf, _SolarWTRuecklauf, _SolarWTVorlauf,
                       _SpeicherA1, _SpeicherA2, _SpeicherA3, _SpeicherA4, _SpeicherA5,
                       _ThermeRuecklaufTempIst, _ThermeVorlaufTempIst,
                       _WWPumpeProzent, _WWRuecklaufTemp, _WWSpeicherTemp1, _WWSpeicherTemp2, _WWVorlaufTemp };

  #define ValueUnknown -10000
  /*
  data Values[] = {{ 0, ValueUnknown, FHEM_HKAnforderung              }, // 0
                   { 0, ValueUnknown, FHEM_HKRuecklaufTemp2           },  
                   { 0, ValueUnknown, FHEM_HKVorlaufTemp1             }, 
                 { 0, ValueUnknown, FHEM_HKVorlaufTemp2             }, // 5
                 { 0, ValueUnknown, FHEM_HKVorlaufTempSoll          }, // 5
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

                 { 0, ValueUnknown, FHEM_SteuerungMemFree           }, 
                 { 0, ValueUnknown, FHEM_SteuerungStatus            }, //20

                 { 0, ValueUnknown, FHEM_ThermeAussenTemp           }, 
                 { 0, ValueUnknown, FHEM_ThermeBetriebsart          }, 
                 { 0, ValueUnknown, FHEM_ThermeBrennerLeistung      }, 
                 { 0, ValueUnknown, FHEM_ThermeKesselTempIst        }, 
                 { 0, ValueUnknown, FHEM_ThermeKesselTempSoll       }, //25
                 { 0, ValueUnknown, FHEM_ThermeRuecklaufTempIst     }, 
                 { 0, ValueUnknown, FHEM_ThermeRuecklaufTempOpto    }, 
                 { 0, ValueUnknown, FHEM_ThermeSpeicherTemp         }, 
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilOpto   }, 
                 { 0, ValueUnknown, FHEM_ThermeUmschaltventilTaster }, //30
                 { 0, ValueUnknown, FHEM_ThermeVorlaufTempIst       }, 
                 { 0, ValueUnknown, FHEM_ThermeVorlaufTempSoll      },  
                 { 0, ValueUnknown, FHEM_ThermeVorlaufValue         }, 
                 { 0, ValueUnknown, FHEM_ThermeVorlaufVentil        },  
                 
                 { 0, ValueUnknown, FHEM_WWAnforderung              }, 
                 { 0, ValueUnknown, FHEM_WWPumpeProzent             }, 
                 { 0, ValueUnknown, FHEM_WWRuecklaufTemp            }, //35
                 { 0, ValueUnknown, FHEM_WWSpeicherTemp1            }, 
                 { 0, ValueUnknown, FHEM_WWSpeicherTemp2            },  
                 { 0, ValueUnknown, FHEM_WWSpeicherTempSoll         },  
                 { 0, ValueUnknown, FHEM_WWVentil                   },  
                 { 0, ValueUnknown, FHEM_WWVorlaufTemp              },

                 { 0, ValueUnknown, FHEM_UNUSED                     }, 
                 { 0, ValueUnknown, FHEM_WWZirkulation              }
                };
*/
#define ThermeBetriebsartAbschalten       0
#define ThermeBetriebsartWW               1
#define ThermeBetriebsartHeizenUndWW      2
#define ThermeBetriebsartDauerndReduziert 3
#define ThermeBetriebsartDauerndNormal    4

#define AnforderungNOT_INITIALIZED            0
#define AnforderungFALSE_Temp1_UNKNOWN       10
#define AnforderungFALSE_Temp1               20
#define AnforderungFALSE_Temp2_UNKNOWN       30
#define AnforderungFALSE_Temp2               40
#define AnforderungFALSE_Temp3_UNKNOWN       50
#define AnforderungFALSE_Temp3               60
#define AnforderungFALSE_Zeitlimit           80
#define AnforderungFALSE_Ausschaltkriterien  90
#define AnforderungFALSE_AusTemp1            91
#define AnforderungFALSE_AusTemp2            92
#define AnforderungFALSE_AusTemp3            93
#define AnforderungTRUE                     100
#define AnforderungTRUE_Temp1               110
#define AnforderungTRUE_Temp2               120
#define AnforderungTRUE_Temp3               130

#define KollektorStatus_UNKNOWN_Uhrzeit              -40
#define KollektorStatus_UNKNOWN_KollektorWTVorlauf   -30
#define KollektorStatus_UNKNOWN_SpeicherA5           -20
#define KollektorStatus_Stagnation                   -10
#define KollektorStatus_Sperrzeit                     -5
#define KollektorStatus_AUS                            0
#define KollektorStatus_TRY                           10
#define KollektorStatus_AN                            20
#define KollektorStatus_AUSschalten                   30

#define  HKtMax                   600
#define  HKHysterese              20
#define  HKSpreizung              30

#define  WWtMax                   570
#define  WWHysterese              20
#define  WWSpreizung              30

char buffer[30]; //Allgemeiner Buffer 

int16_t ValueX10new1;
uint8_t tmpUint8_1, tmpUint8_2, tmpUint8_3;
uint16_t tmpUint16_1;
int32_t tmpInt32_1, tmpInt32_2;

#define EEPROM_Offset_Stagnation   0
#define EEPROM_Offset_owArray    100

//#define MIN(a,b)			((a<b)?(a):(b))
//#define MAX(a,b)			((a>b)?(a):(b))
//#define ABS(x)				((x>0)?(x):(-x))
