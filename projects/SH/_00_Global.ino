typedef struct {
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
} data;

#define       _HKAnforderung               0
#define       _HKRuecklaufTemp2            1
#define       _HKVorlaufTemp1              2
#define       _HKVorlaufTemp2              3
#define       _HKVorlaufTempSoll           4
#define       _HKVorlaufValue              5

#define       _KollektorAusdehnung         6
#define       _KollektorStatus             7
#define       _KollektorWTRuecklauf        8
#define       _KollektorWTVorlauf          9        

#define       _SolarRuecklauf             10
#define       _SolarStatus                11
#define       _SolarVorlauf               12
#define       _SolarWTRuecklauf           13
#define       _SolarWTVorlauf             14   

#define       _SpeicherA1                 15
#define       _SpeicherA2                 16
#define       _SpeicherA3                 17
#define       _SpeicherA4                 18
#define       _SpeicherA5                 19

#define       _SteuerungMemFree           20
#define       _ALT_SteuerungStatus_ALT    21

#define       _ThermeAussenTemp           22
#define       _ThermeBetriebsart          23
#define       _ThermeBrennerLeistung      24
#define       _ThermeKesselTempIst        25
#define       _ThermeKesselTempSoll       26
#define       _ThermeRuecklaufTempIst     27
#define       _ThermeRuecklaufTempOpto    28
#define       _ThermeSpeicherTemp         29
#define       _ThermeUmschaltventilOpto   30
#define       _ThermeUmschaltventilTaster 31
#define       _ThermeVorlaufTempIst       32
#define       _ThermeVorlaufTempSoll      33
#define       _ThermeVorlaufValue         34
#define       _ThermeVorlaufVentil        35

#define       _WWAnforderung              36
#define       _WWPumpeProzent             37
#define       _WWRuecklaufTemp            38
#define       _WWSpeicherTemp1            39
#define       _WWSpeicherTemp2            40
#define       _WWSpeicherTempSoll         41
#define       _WWVentil                   42
#define       _WWVorlaufTemp              43
#define       _KollektorDach              44
#define       _WWZirkulation              45

data Values[46];

uint8_t  owArray[] = { _HKRuecklaufTemp2, _HKVorlaufTemp1, _HKVorlaufTemp2,
                       _KollektorAusdehnung,  _KollektorWTRuecklauf,_KollektorWTVorlauf,
                       _SolarRuecklauf, _SolarVorlauf, _SolarWTRuecklauf, _SolarWTVorlauf,
                       _SpeicherA1, _SpeicherA2, _SpeicherA3, _SpeicherA4, _SpeicherA5,
                       _ThermeRuecklaufTempIst, _ThermeVorlaufTempIst,
                       _WWRuecklaufTemp, _WWSpeicherTemp1, _WWSpeicherTemp2, _WWVorlaufTemp };
                       
uint8_t dpAskArray[] = { _KollektorDach, _HKVorlaufTempSoll, _WWSpeicherTempSoll};

#define ValueUnknown -10000

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

