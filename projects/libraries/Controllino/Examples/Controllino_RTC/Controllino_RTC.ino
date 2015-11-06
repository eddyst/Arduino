#include <SPI.h>

#include <Controllino.h>


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  Controllino_RTC_init(0);

  //Controllino_SetTimeDate(31,2,12,99,23,41,23); // set initial values to the RTC chip
}

// the loop routine runs over and over again forever:
void loop() {
  int n;  
  Serial.print("Day: ");n = Controllino_GetDay(); Serial.println(n);
  
  Serial.print("WeekDay: ");n = Controllino_GetWeekDay(); Serial.println(n);
  
  Serial.print("Month: ");n = Controllino_GetMonth(); Serial.println(n);

  Serial.print("Year: ");n = Controllino_GetYear(); Serial.println(n);

  Serial.print("Hour: ");n = Controllino_GetHour(); Serial.println(n);

  Serial.print("Minute: "); n = Controllino_GetMinute(); Serial.println(n);

  Serial.print("Second: ");n = Controllino_GetSecond(); Serial.println(n);
 
  delay(5000);        
}

