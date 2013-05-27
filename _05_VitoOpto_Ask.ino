void VitoAsk () {
  static uint8_t VitoAdr = 0;
  if (VitoAdr > 8) {
    static uint16_t LastRead;
    uint16_t mill = millis();
    if (mill-LastRead > 30000) { // Nur alle ? Sek die Abfrage gegen die Heizung starten
      VitoAdr = 0;               // Es wird ja vor der Auswertung dazugezählt
      LastRead=mill;
    }
  } 
  else
  {
    switch (++VitoAdr) {
    case  1: // KesselSollTemperatur (/ 10)
      Vito.beginReadValue(0x55, 0x5A, 0x02); 
      break; 
    case  2: // KesselTemperatur (/ 100)
      Vito.beginReadValue(0xA3, 0x93, 0x02); 
      break; 
    case  3: // Vorlaufsolltemperatur A1M1 (/ 10)
      Vito.beginReadValue(0x25, 0x44, 0x02); 
      break; 
    case  4: // Ruecklauftemperatur (17A) RLTS (/ 10)
      Vito.beginReadValue(0x08, 0x08, 0x02); 
      break; 
    case  5: // Speichertemperatur Tiefpass STS1 (/ 10)
      Vito.beginReadValue(0x08, 0x12, 0x02); 
      break; 
    case  6: // Aussentemperatur Tiefpass (/ 10)
      Vito.beginReadValue(0x55, 0x25, 0x02); 
      break; 
    case  7: // Betriebsart A1M1
      Vito.beginReadValue(0x23, 0x23, 0x01); 
      break; 
    case  8: // Umschaltventil (0..3)
      Vito.beginReadValue(0x0A, 0x10, 0x01); 
      break; 
    case  9: // Anlagen Ist-Leistung (0..200 / 0..1)
      Vito.beginReadValue(0xA3, 0x8F, 0x02); 
      break; 
/*    case  : // Brennerstarts
      Vito.beginReadValue(0x08, 0x8A, 0x04); 
      break; 
    case : // Betriebsstunden Stufe 1 (Sek)
      Vito.beginReadValue(0x08, 0x86, 0x04); 
      break; 
    case : // Sammelstörung (0..1)
      Vito.beginReadValue(0x0A, 0x82, 0x01); 
      break; 
    case : // Sparbetrieb A1M1 (0..1)
      Vito.beginReadValue(0x23, 0x02, 0x01); 
      break; 
    case : // Partybetrieb A1M1 (0..1)
      Vito.beginReadValue(0x23, 0x03, 0x01); 
      break; 
    case : // Speicherladepumpe (0..1)
      Vito.beginReadValue(0x65, 0x13, 0x01); 
      break; 
    case : // Interne Pumpe (0..1 / 0..100)
      Vito.beginReadValue(0x76, 0x60, 0x02); 
      break; 
    case : // Heizkreispumpe A1M1 (0..1 / 0..100)
      Vito.beginReadValue(0x76, 0x63, 0x02); 
      break; 
*/
/*
    case  ?: // aktuelle Betriebsart A1M1
       Vito.beginReadValue(0x25, 0x00, 0x16); 
       break; 
       case  ?: // KesselTemperatur Tiefpass (/ 10)
       Vito.beginReadValue(0x08, 0x10, 0x02); 
       break; 
       case  ?: // Aussentemperatur gedämpft (/ 10)
       Vito.beginReadValue(0x55, 0x27, 0x02); 
       break; 
*/
    default: 
      Debug.println();
      Debug.print("Unbekannte VitoAdr:");
      Debug.println(VitoAdr);
    }
  }
}

/*
uint8_t Address_01[] = { 0x25, 0x00, 0x16}; // aktuelle Betriebsart A1M1
 uint8_t Address_02[] = { 0x55, 0x5A, 0x02}; // Kesselsolltemperatur (/ 10)
 uint8_t Address_03[] = { 0xA3, 0x93, 0x02}; // Kesseltemperatur (/ 100)
 uint8_t Address_04[] = { 0x08, 0x10, 0x02}; // Kesseltemperatur Tiefpass (/ 10)
 uint8_t Address_05[] = { 0x25, 0x44, 0x02}; // Vorlaufsolltemperatur A1M1 (/ 10)
 uint8_t Address_06[] = { 0x08, 0x08, 0x02}; // Rücklauftemperatur (17A) RLTS (/ 10)
 uint8_t Address_07[] = { 0x08, 0x12, 0x02}; // Speichertemperatur Tiefpass STS1 (/ 10)
 uint8_t Address_08[] = { 0x55, 0x25, 0x02}; // Aussentemperatur Tiefpass (/ 10)
 uint8_t Address_09[] = { 0x55, 0x27, 0x02}; // Aussentemperatur gedämpft (/ 10)
 uint8_t Address_10[] = { 0x0A, 0x10, 0x01}; // Umschaltventil (0..3)
 uint8_t Address_11[] = { 0x08, 0x8A, 0x04}; // Brennerstarts
 uint8_t Address_12[] = { 0x08, 0x86, 0x04}; // Betriebsstunden Stufe 1 (Sek)
 uint8_t Address_13[] = { 0x23, 0x23, 0x01}; // Betriebsart A1M1
 uint8_t Address_14[] = { 0x0A, 0x82, 0x01}; // Sammelstörung (0..1)
 uint8_t Address_15[] = { 0x23, 0x02, 0x01}; // Sparbetrieb A1M1 (0..1)
 uint8_t Address_16[] = { 0x23, 0x03, 0x01}; // Partybetrieb A1M1 (0..1)
 uint8_t Address_17[] = { 0x65, 0x13, 0x01}; // Speicherladepumpe (0..1)
 uint8_t Address_18[] = { 0x76, 0x60, 0x02}; // Interne Pumpe (0..1 / 0..100)
 uint8_t Address_19[] = { 0x76, 0x63, 0x02}; // Heizkreispumpe A1M1 (0..1 / 0..100)
 uint8_t Address_20[] = { 0xA3, 0x8F, 0x02}; // Anlagen Ist-Leistung (0..200 / 0..1)
 */

