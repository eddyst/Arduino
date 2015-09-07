void ethDoAsk() {
    static uint8_t Task  = 0;
    static uint8_t Step = 0;
    switch (Task){
    case 0:
      static uint32_t lastOwAddrSync = millis() - owAddrSyncIntervall;
      if (millis() - lastOwAddrSync > owAddrSyncIntervall) { //jede Stunde Addressen neu abfragen
        if (Step >= sizeof(owArray)) {
          lastOwAddrSync = millis();
          Task++; 
          Step = 0;
        } 
        else{
          //---ALT------
          //Bsp: sending: {"<OWA1=".AttrVal("HKRuecklauf","ID","0 0 0 0 0 0 0 0").">" }
          //     returns: <OWA1=00 00 00 00 00 00 00 00> 
          //---NEU------
          //Bsp: sending: dp001id
          //     returns: <dp001id=00 00 00 00 00 00 00 00> 
          if (ethLogLevel > 1) Debug.print( F( "eth: OUT: dp"));
          client.print( F( "dp"));
          if (owArray[Step] < 100) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (owArray[Step] < 10) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (ethLogLevel > 1) Debug.print( owArray[Step]);  
          client.print( owArray[Step]); 
          if (ethLogLevel > 1) Debug.println( F( "id"));
          client.println( F( "id"));
          AskWait = true;
          Step ++;
        }
      } 
      else Task++; 
      break;
    case 1:
      static uint32_t lastdpAskArray = millis() - dpAskArraySyncIntervall;
      if (millis() - lastdpAskArray > dpAskArraySyncIntervall) { //jede Stunde Addressen neu abfragen
        if (Step >= sizeof(dpAskArray)) {
          lastdpAskArray = millis();
          Task++; 
          Step = 0;
        } else {
          //Bsp: sending: dp044get
          //     returns: <dp044=123> 
          //oder returns: <dp044=!> wenn keine Daten vorhanden sind, die aktuell genug sind (Definition im cmdalias auf FHEM)
          if (ethLogLevel > 1) Debug.print( F( "eth: OUT: dp"));
          client.print( F( "dp"));
          if (dpAskArray[Step] < 100) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (dpAskArray[Step] < 10) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (ethLogLevel > 1) Debug.print( dpAskArray[Step]);  
          client.print( dpAskArray[Step]); 
          if (ethLogLevel > 1) Debug.println( F( "get"));
          client.println( F( "get"));
          AskWait = true;
          Step ++;
        }
      }
      else Task++; 
      break;
    case 2:    
      if (Step >= sizeof(Values)/sizeof(data)) {
        Task++; 
        Step = 0;
      } 
      else{
        if ( Values[Step].Changed == 1 && Values[Step].ValueX10 != ValueUnknown) {
          Values[Step].Changed = 0;
          if (ethLogLevel > 1) Debug.print( F("eth: OUT: dp"));  
          client.print( F( "dp"));
          if (Step < 100) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (Step < 10) {
            if (ethLogLevel > 1) Debug.print( "0");  
            client.print( "0"); 
          }
          if (ethLogLevel > 1) Debug.print( Step);  
          client.print( Step); 
          if (ethLogLevel > 1) Debug.print( F("set "));  
          client.print( F( "set ")); 
          if (ethLogLevel > 1) Debug.println( ( float)Values[Step].ValueX10 / 10, 1);
          client.println( ( float)Values[Step].ValueX10 / 10, 1);
          AskWait = true;
        }
        Step ++;
      }
      break; 
    default:
      Task=0;
    }
}  
