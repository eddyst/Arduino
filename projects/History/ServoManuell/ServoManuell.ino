// Festlegen des Servo Pins
int servo = 4;

// Weitere Variablen für PWM und Drehwinkel (Position)
int pwm;
int pos = 1000;
int d = 1,i;
/* Funktion um das Servo zu bewegen. Hier wird die Position
berechnet und der Wert der neuen Position an das Servo
übergeben. */

// Initialisieren des Servo Pins
void setup() {
   pinMode(servo, OUTPUT);
   Serial.begin(57600);
   
}
uint32_t servoTimer, moveTimer;
// Hauptprogramm
void loop() {
  if (millis() - servoTimer >= 20) {
    servoTimer = millis();
    if (millis() - moveTimer >= 500) {
      moveTimer = millis();
      pos = pos + 10 * d;
      if (pos <=  550) d =  1;
      if (pos >= 1800) d = -1;
      Serial.print   (d);
      Serial.print   (" - ");
      Serial.println (pos);
    }
    // Servo Pin auf HIGH zum aktivieren des Servos
    digitalWrite(servo, HIGH);
    // Kurze Zeit warten
    delayMicroseconds(pos);
    // Servo Pin auf LOW zum deaktivieren des servos
    digitalWrite(servo, LOW);
  }
}
