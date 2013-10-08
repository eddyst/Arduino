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
