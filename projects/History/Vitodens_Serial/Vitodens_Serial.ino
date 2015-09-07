// This is written for MEGA32 on a AVR-Net-Io Board

int Pin=0;

void setup() {
  Serial.begin(4800);
  UCSRC =  (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1)|(1<<UPM1)|(1<<USBS); //ins UCSRC Register schreiben
  pinMode(Pin, OUTPUT);
}

void loop() {
  static boolean P;
  static byte I;
  digitalWrite( Pin, P++);
  Serial.write (I++);
  delay(1000);
}
