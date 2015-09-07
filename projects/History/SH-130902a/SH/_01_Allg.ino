#define NotbetriebPin               22

#include <Bounce.h>
#include <Clock.h>
Clock g_Clock;

void AllgInit() {
  pinMode     (NotbetriebPin              , INPUT_PULLUP); //Kein Schalter = Schalter offen = Pin HIGH = Notbetrieb
  g_Clock.SetTimezoneOffset(1,0); // Setup the clock. In Germany we are 1 hours ahead of GMT 
  InitValue (_HKAnforderung              , FHEM_HKAnforderung              ); setValue( _HKAnforderung, AnforderungNOT_INITIALIZED);
  InitValue (_HKRuecklaufTemp2           , FHEM_HKRuecklaufTemp2           );  
  InitValue (_HKRuecklaufTemp1Mieter     , FHEM_HKRuecklaufTemp1Mieter     ); 
  InitValue (_HKRuecklaufTemp1Steier     , FHEM_HKRuecklaufTemp1Steier     ); 
  InitValue (_HKVorlaufTemp1             , FHEM_HKVorlaufTemp1             ); 
  InitValue (_HKVorlaufTemp2             , FHEM_HKVorlaufTemp2             );
  InitValue (_HKVorlaufTempSoll          , FHEM_HKVorlaufTempSoll          ); setValue( _HKVorlaufTempSoll, 300);
  InitValue (_HKVorlaufValue             , FHEM_HKVorlaufValue             ); 

  InitValue (_KollektorAusdehnung        , FHEM_KollektorAusdehnung        ); 
  InitValue (_KollektorStatus            , FHEM_KollektorStatus            ); 
  InitValue (_KollektorWTRuecklauf       , FHEM_KollektorWTRuecklauf       ); 
  InitValue (_KollektorWTVorlauf         , FHEM_KollektorWTVorlauf         );

  InitValue (_SolarRuecklauf             , FHEM_SolarRuecklauf             ); 
  InitValue (_SolarStatus                , FHEM_SolarStatus                ); 
  InitValue (_SolarVorlauf               , FHEM_SolarVorlauf               ); 
  InitValue (_SolarWTRuecklauf           , FHEM_SolarWTRuecklauf           ); 
  InitValue (_SolarWTVorlauf             , FHEM_SolarWTVorlauf             );

  InitValue (_SpeicherA1                 , FHEM_SpeicherA1                 ); 
  InitValue (_SpeicherA2                 , FHEM_SpeicherA2                 ); 
  InitValue (_SpeicherA3                 , FHEM_SpeicherA3                 ); 
  InitValue (_SpeicherA4                 , FHEM_SpeicherA4                 ); 
  InitValue (_SpeicherA5                 , FHEM_SpeicherA5                 );
  
  InitValue (_SteuerungMemFree           , FHEM_SteuerungMemFree           ); 
  InitValue (_SteuerungStatus            , FHEM_SteuerungStatus            ); setValue( _SteuerungStatus, 0);

  InitValue (_ThermeAussenTemp           , FHEM_ThermeAussenTemp           ); 
  InitValue (_ThermeBetriebsart          , FHEM_ThermeBetriebsart          ); 
  InitValue (_ThermeBrennerLeistung      , FHEM_ThermeBrennerLeistung      ); 
  InitValue (_ThermeKesselTempIst        , FHEM_ThermeKesselTempIst        ); 
  InitValue (_ThermeKesselTempSoll       , FHEM_ThermeKesselTempSoll       );
  InitValue (_ThermeRuecklaufTemp        , FHEM_ThermeRuecklaufTemp        ); 
  InitValue (_ThermeRuecklaufTempOpto    , FHEM_ThermeRuecklaufTempOpto    ); 
  InitValue (_ThermeSpeicherTemp         , FHEM_ThermeSpeicherTemp         ); 
  InitValue (_ThermeUmschaltventilOpto   , FHEM_ThermeUmschaltventilOpto   ); 
  InitValue (_ThermeUmschaltventilTaster , FHEM_ThermeUmschaltventilTaster );
  InitValue (_ThermeVorlaufTempIst       , FHEM_ThermeVorlaufTempIst       ); 
  InitValue (_ThermeVorlaufTempSoll      , FHEM_ThermeVorlaufTempSoll      );  
  InitValue (_ThermeVorlaufValue         , FHEM_ThermeVorlaufValue         ); setValue( _ThermeVorlaufValue, 0);
  InitValue (_ThermeVorlaufVentil        , FHEM_ThermeVorlaufVentil        );  

  InitValue (_WWAnforderung              , FHEM_WWAnforderung              ); setValue( _WWAnforderung, AnforderungNOT_INITIALIZED);
  InitValue (_WWPumpeProzent             , FHEM_WWPumpeProzent             ); 
  InitValue (_WWRuecklaufTemp            , FHEM_WWRuecklaufTemp            );
  InitValue (_WWSpeicherTemp1            , FHEM_WWSpeicherTemp1            ); 
  InitValue (_WWSpeicherTemp2            , FHEM_WWSpeicherTemp2            );  
  InitValue (_WWSpeicherTempSoll         , FHEM_WWSpeicherTempSoll         ); setValue( _WWSpeicherTempSoll, 450);
  InitValue (_WWVentil                   , FHEM_WWVentil                   );  
  InitValue (_WWVorlaufTemp              , FHEM_WWVorlaufTemp              );
}


void   AllgDoEvents() {
  setValue( _SteuerungMemFree, freeMemory());
  g_Clock.Maintain();
  pruefeNotbetriebPin();
}

void   pruefeNotbetriebPin() {
  static Bounce bouncer = Bounce(NotbetriebPin, 500, true); 
  if (bouncer.update()) {
    if (bouncer.fallingEdge()) {
      ThermeSolarbetriebBeginnt();
      HKSolarbetriebBeginnt();    
    } 
    else if (bouncer.risingEdge()) {
      ThermeSolarbetriebEndet();
      HKSolarbetriebEndet();
    } 
  }
}

void InitValue(uint8_t index, prog_char *name) {
  Values[index].Changed = 0;
  Values[index].ValueX10 = ValueUnknown;
  Values[index].Name = name;
}

boolean setValue( uint8_t valueID, int16_t value) {
  if (Values[valueID].ValueX10 != value) {
    Values[valueID].ValueX10 = value;
    Values[valueID].Changed  = 1;
    return true;
  }  else {
    return false;
  }
}

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

boolean iif(const boolean value, uint8_t TrueValue, uint8_t FalseValue) {
  if (value) 
    return TrueValue;
  else
    return FalseValue;
}









