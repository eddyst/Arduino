#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

volatile char daten;

void modbus_init(uint32_t baud, uint8_t data, char parity, uint8_t stop){
// UART Initialisierung
 uint16_t ubrr = ((F_CPU+baud*8)/(baud*16)-1);

// Baudrate einstellen
 UBRRH = (uint8_t)(ubrr>>8);
 UBRRL = (uint8_t)(ubrr);

 UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);// | (1<<TXCIE); 

  switch (data){
    case 5:            // 5 Datenbits   
      UCSRC = (1<<URSEL);   
      break;
    case 6:            // 6 Datenbits
      UCSRC = (1<<URSEL) | (1<<UCSZ0);
      break;
    case 7:            // 7 Datenbits
      UCSRC = (1<<URSEL) |  (1<<UCSZ1);
      break;
    case 8:            // 8 Datenbits
      UCSRC = (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
      break;
  } 
  
  switch (parity){
    case 'N':            // Parity NONE
      break;
    case 'E':            // Parity EVEN
      UCSRC |= (1<<URSEL) | (1<<UPM1);
      break;
    case 'O':            // Parity ODD
      UCSRC |= (1<<URSEL) | (1<<UPM0) | (1<<UPM1);
      break;
  }

  if(stop == 2) UCSRC |= (1<<URSEL) | (1<<USBS);

  sei();
}

void send_char(char data) {
 while (!(UCSRA & (1<<UDRE)));
 UDR = data;
}

void send_string(char *data) {
 while (*data) { /* so lange *data != '\0' also ungleich dem "String-Endezeichen" */
  send_char(*data);
  data++;
 };
}

ISR(USART_RXC_vect){
 daten = UDR;
 send_char(daten);
// send_string(" - wurde empfangen\n\r");
}

int main(void){
 modbus_init(4800,8,'E',2);
 while(1);
 return 0;
}
