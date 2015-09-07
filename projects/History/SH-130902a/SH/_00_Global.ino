typedef struct {
//  uint8_t Changed  ;  int16_t ValueX10 ;
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
  prog_char *Name;
} data;

#define       _HKAnforderung               0
prog_char FHEM_HKAnforderung              [] myPROGMEM =  "HKAnforderung";
#define       _HKRuecklaufTemp1Mieter      2
prog_char FHEM_HKRuecklaufTemp1Mieter     [] myPROGMEM =  "HKRuecklaufTemp1Mieter";
#define       _HKRuecklaufTemp1Steier      3
prog_char FHEM_HKRuecklaufTemp1Steier     [] myPROGMEM =  "HKRuecklaufTemp1Steier";
#define       _HKRuecklaufTemp2            1
prog_char FHEM_HKRuecklaufTemp2           [] myPROGMEM =  "HKRuecklaufTemp2";
#define       _HKVorlaufTemp1              4
prog_char FHEM_HKVorlaufTemp1             [] myPROGMEM =  "HKVorlaufTemp1";
#define       _HKVorlaufTemp2              5
prog_char FHEM_HKVorlaufTemp2             [] myPROGMEM =  "HKVorlaufTemp2";
#define       _HKVorlaufTempSoll          40
prog_char FHEM_HKVorlaufTempSoll          [] myPROGMEM =  "HKVorlaufTempSoll";
#define       _HKVorlaufValue              6
prog_char FHEM_HKVorlaufValue             [] myPROGMEM =  "HKVorlaufValue";

#define       _KollektorAusdehnung         7
prog_char FHEM_KollektorAusdehnung        [] myPROGMEM =  "KollektorAusdehnung";
#define       _KollektorStatus             8
prog_char FHEM_KollektorStatus            [] myPROGMEM =  "KollektorStatus";
#define       _KollektorWTRuecklauf        9
prog_char FHEM_KollektorWTRuecklauf       [] myPROGMEM =  "KollektorWTRuecklauf";
#define       _KollektorWTVorlauf         10        
prog_char FHEM_KollektorWTVorlauf         [] myPROGMEM =  "KollektorWTVorlauf";

#define       _SolarRuecklauf             11
prog_char FHEM_SolarRuecklauf             [] myPROGMEM =  "SolarRuecklauf";
#define       _SolarStatus                12
prog_char FHEM_SolarStatus                [] myPROGMEM =  "SolarStatus";
#define       _SolarVorlauf               13
prog_char FHEM_SolarVorlauf               [] myPROGMEM =  "SolarVorlauf";
#define       _SolarWTRuecklauf           14
prog_char FHEM_SolarWTRuecklauf           [] myPROGMEM =  "SolarWTRuecklauf";
#define       _SolarWTVorlauf             15   
prog_char FHEM_SolarWTVorlauf             [] myPROGMEM =  "SolarWTVorlauf";

#define       _SpeicherA1                 16
prog_char FHEM_SpeicherA1                 [] myPROGMEM =  "SpeicherA1";
#define       _SpeicherA2                 17
prog_char FHEM_SpeicherA2                 [] myPROGMEM =  "SpeicherA2";
#define       _SpeicherA3                 18
prog_char FHEM_SpeicherA3                 [] myPROGMEM =  "SpeicherA3";
#define       _SpeicherA4                 19
prog_char FHEM_SpeicherA4                 [] myPROGMEM =  "SpeicherA4";
#define       _SpeicherA5                 20
prog_char FHEM_SpeicherA5                 [] myPROGMEM =  "SpeicherA5";

#define       _SteuerungMemFree           44
prog_char FHEM_SteuerungMemFree           [] myPROGMEM =  "SteuerungMemFree";
#define       _SteuerungStatus            43
prog_char FHEM_SteuerungStatus            [] myPROGMEM =  "SteuerungStatus";

#define       _ThermeAussenTemp           21
prog_char FHEM_ThermeAussenTemp           [] myPROGMEM =  "ThermeAussenTemp";
#define       _ThermeBetriebsart          22
prog_char FHEM_ThermeBetriebsart          [] myPROGMEM =  "ThermeBetriebsart";
#define       _ThermeBrennerLeistung      23
prog_char FHEM_ThermeBrennerLeistung      [] myPROGMEM =  "ThermeBrennerLeistung";
#define       _ThermeKesselTempIst        24
prog_char FHEM_ThermeKesselTempIst        [] myPROGMEM =  "ThermeKesselTempIst";
#define       _ThermeKesselTempSoll       25
prog_char FHEM_ThermeKesselTempSoll       [] myPROGMEM =  "ThermeKesselTempSoll";
#define       _ThermeRuecklaufTemp        26
prog_char FHEM_ThermeRuecklaufTemp        [] myPROGMEM =  "ThermeRuecklaufTemp";
#define       _ThermeRuecklaufTempOpto    27
prog_char FHEM_ThermeRuecklaufTempOpto    [] myPROGMEM =  "ThermeRuecklaufTempOpto";
#define       _ThermeSpeicherTemp         28
prog_char FHEM_ThermeSpeicherTemp         [] myPROGMEM =  "ThermeSpeicherTemp";
#define       _ThermeUmschaltventilOpto   29
prog_char FHEM_ThermeUmschaltventilOpto   [] myPROGMEM =  "ThermeUmschaltventilOpto";
#define       _ThermeUmschaltventilTaster 30
prog_char FHEM_ThermeUmschaltventilTaster [] myPROGMEM =  "ThermeUmschaltventilTaster";
#define       _ThermeVorlaufTempIst       31
prog_char FHEM_ThermeVorlaufTempIst       [] myPROGMEM =  "ThermeVorlaufTempIst";
#define       _ThermeVorlaufTempSoll      32
prog_char FHEM_ThermeVorlaufTempSoll      [] myPROGMEM =  "ThermeVorlaufTempSoll";
#define       _ThermeVorlaufValue         42
prog_char FHEM_ThermeVorlaufValue         [] myPROGMEM =  "ThermeVorlaufValue";
#define       _ThermeVorlaufVentil        45
prog_char FHEM_ThermeVorlaufVentil        [] myPROGMEM =  "ThermeVorlaufVentil";

#define       _WWAnforderung              33
prog_char FHEM_WWAnforderung              [] myPROGMEM =  "WWAnforderung";
#define       _WWPumpeProzent             34
prog_char FHEM_WWPumpeProzent             [] myPROGMEM =  "WWPumpeProzent";
#define       _WWRuecklaufTemp            35
prog_char FHEM_WWRuecklaufTemp            [] myPROGMEM =  "WWRuecklaufTemp";
#define       _WWSpeicherTemp1            36
prog_char FHEM_WWSpeicherTemp1            [] myPROGMEM =  "WWSpeicherTemp1";
#define       _WWSpeicherTemp2            37
prog_char FHEM_WWSpeicherTemp2            [] myPROGMEM =  "WWSpeicherTemp2";
#define       _WWSpeicherTempSoll         41
prog_char FHEM_WWSpeicherTempSoll         [] myPROGMEM =  "WWSpeicherTempSoll";
#define       _WWVentil                   38
prog_char FHEM_WWVentil                   [] myPROGMEM =  "WWVentil";
#define       _WWVorlaufTemp              39
prog_char FHEM_WWVorlaufTemp              [] myPROGMEM =  "WWVorlaufTemp";

data Values[46];

uint8_t  owArray[] = { _HKRuecklaufTemp2, _HKRuecklaufTemp1Mieter, _HKRuecklaufTemp1Steier, _HKVorlaufTemp1, _HKVorlaufTemp2,
                       _KollektorAusdehnung,  _KollektorWTRuecklauf,_KollektorWTVorlauf,
                       _SolarRuecklauf, _SolarVorlauf, _SolarWTRuecklauf, _SolarWTVorlauf,
                       _SpeicherA1, _SpeicherA2, _SpeicherA3, _SpeicherA4, _SpeicherA5,
                       _ThermeRuecklaufTemp, _ThermeVorlaufTempIst,
                       _WWPumpeProzent, _WWRuecklaufTemp, _WWSpeicherTemp1, _WWSpeicherTemp2, _WWVorlaufTemp };

#define ValueUnknown -10000

#define AnforderungNOT_INITIALIZED            0
#define AnforderungFALSE_Temp1_UNKNOWN       10
#define AnforderungFALSE_Temp1               20
#define AnforderungFALSE_Temp2_UNKNOWN       30
#define AnforderungFALSE_Temp2               40
#define AnforderungFALSE_Temp3_UNKNOWN       50
#define AnforderungFALSE_Temp3               60
//#define AnforderungFALSE_ZeitlimitStart      70
#define AnforderungFALSE_Zeitlimit           80
#define AnforderungFALSE_Ausschaltkriterien  90
#define AnforderungFALSE_AusTemp1            91
#define AnforderungFALSE_AusTemp2            92
#define AnforderungFALSE_AusTemp3            93
#define AnforderungTRUE                     100
#define AnforderungTRUE_Temp2               110
#define AnforderungTRUE_Temp3               120

#define  HKtMax                   600
#define  HKHysterese              20
#define  HKSpreizung              30

#define  WWtMax                   570
#define  WWHysterese              20
#define  WWSpreizung              30

         #define ThermeBetriebsartAbschalten       0
         #define ThermeBetriebsartWW               1
         #define ThermeBetriebsartHeizenUndWW      2
         #define ThermeBetriebsartDauerndReduziert 3
         #define ThermeBetriebsartDauerndNormal    4

char buffer[30]; //Allgemeiner Buffer 
int16_t ValueX10new1;

#define EEPROM_Offset_Stagnation   0
#define EEPROM_Offset_owArray    100


