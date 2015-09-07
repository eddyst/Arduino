#include "SerialBuster.h"
#include "Buffer.h"

SerialBuster sb = SerialBuster(256, 256, 128);

void   RS485Init() {
//  sb.init(115200);
//  sb.setCallback(incomingPacket);
//  sb.setAddress(1);
}
void RS485DoEvents() {
//  sb.update();

}

//void incomingPacket (uint8_t from, Buffer * buffer) {

//}

/*
Beim initialisieren wird max.Anz mal timeslot 
wenn ein Wert kommt, wird die differenz zu dem wert den wir senden wollen * timeslot gewartet
bei Ablauf des Timer, senden wir

Es hat also jeder Wert einen Timeslot in dem er gesendet werden kann. Die zeitliche Syncronisation ergibt sich aus dem empfang von Werten
*/
