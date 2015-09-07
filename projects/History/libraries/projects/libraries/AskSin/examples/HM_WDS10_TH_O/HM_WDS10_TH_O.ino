//#define SER_DBG


//- load library's --------------------------------------------------------------------------------------------------------
#include "Register.h"																	// configuration sheet
#include <Buttons.h>																	// remote buttons library
#include <Sensor_SHT10_BMP085.h>

#include <Sensirion.h>																	// sht10 library
#include <Wire.h>																		// i2c library, needed for bmp085 or bmp180
#include <BMP085.h>																		// pressure library

//- serial communication --------------------------------------------------------------------------------------------------
const uint8_t helptext1[] PROGMEM = {													// help text for serial console
	"\n"
	"Available commands:" "\n"
	"  p                - start pairing with master" "\n"
	"  r                - reset device" "\n"
	"  b[0]  b[n]  s    - send a string, b[0] is length (50 bytes max)" "\n"
	"\n"
	"  c                - print configuration" "\n"
	"\n"
	"  $nn for HEX input (e.g. $AB,$AC ); b[] = byte, i[]. = integer " "\n"
};
#ifdef SER_DBG
const InputParser::Commands cmdTab[] PROGMEM = {
	{ 'p', 0, sendPairing },
	{ 'r', 0, resetDevice },
	{ 's', 1, sendCmdStr },
	{ 'b', 1, buttonSend },
	{ 'c', 0, printConfig },
	{ 0 }
};
InputParser parser (50, cmdTab);
#endif


//- homematic communication -----------------------------------------------------------------------------------------------
HM::s_jumptable jTbl[] = {																// jump table for HM communication
	// byte3, byte10, byte11, function to call											// 0xff means - any byte
//	{ 0x01, 0xff, 0x0e, HM_Status_Request },
	{ 0x11, 0x04, 0x00, HM_Reset_Cmd },
//	{ 0x01, 0xff, 0x06, HM_Config_Changed },
//	{ 0x40, 0xff, 0xff, HM_Remote_Event },
	{ 0x00 }
};
Buttons button[1];																		// declare remote button object

Sensirion sht10;
BMP085 bmp085;
Sensor_SHT10_BMP085 sensTHP;


//- main functions --------------------------------------------------------------------------------------------------------
void setup() {
#ifdef SER_DBG
	Serial.begin(57600);																// serial setup
	Serial << F("Starting sketch...\n");												// ...and some information
	Serial << pCharPGM(helptext1) << '\n';
	Serial << F("freeMem: ") << freeMem() << F(" byte") <<'\n';
#endif	

	hm.cc.config(10,11,12,13,2,0);														// CS, MOSI, MISO, SCK, GDO0, Interrupt

	hm.statusLed.config(4, 6);															// configure the status led pin
	hm.statusLed.set(STATUSLED_BOTH, STATUSLED_MODE_BLINKFAST, 3);

	hm.battery.config(1,0,10000);														// set battery measurement
	hm.battery.setVoltage(30);															// voltage to 3.1 volt

	hm.setPowerMode(0);																	// power mode for HM device
	hm.init();																			// initialize the hm module

	button[0].regInHM(0,&hm);															// register buttons in HM per channel, handover HM class pointer
	//pinMode(8,INPUT_PULLUP);
	button[0].config(9, NULL);															// configure button on specific pin and handover a function pointer to the main sketch

	sensTHP.regInHM(1,&hm);																// register sensor class in hm
	sensTHP.config(A4, A5, 0, &sht10, &bmp085, 570);										// data pin, clock pin and timing - 0 means HM calculated timing, every number above will taken in milliseconds
	//sensTHP.config(A4, A5, 0, &sht10, NULL, 570);										// data pin, clock pin and timing - 0 means HM calculated timing, every number above will taken in milliseconds
	
	//uint8_t x[] = {(uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256)};
	//Serial << pHex(x,3) << '\n';
	//Serial << "\npair: " << pHex(regs.ch0.l0.pairCentral,3) << '\n';	
}

void loop() {
#ifdef SER_DBG
	parser.poll();																		// handle serial input from console
#endif
	hm.poll();																			// poll the HM communication
}


//- HM functions ----------------------------------------------------------------------------------------------------------
void HM_Status_Request(uint8_t *data, uint8_t len) {
//	Serial << F("status request, data: ") << pHex(data,len) << '\n';
}
void HM_Reset_Cmd(uint8_t *data, uint8_t len) {
//	Serial << F("reset, data: ") << pHex(data,len) << '\n';
	hm.send_ACK();																		// send an ACK
	if (data[0] == 0) hm.reset();														// do a reset only if channel is 0
}
void HM_Config_Changed(uint8_t *data, uint8_t len) {
//	Serial << F("config changed, data: ") << pHex(data,len) << '\n';
}
void HM_Remote_Event(uint8_t *data, uint8_t len) {
//	Serial << F("remote event, data: ") << pHex(data,len) << '\n';
}


//- config functions ------------------------------------------------------------------------------------------------------
#ifdef SER_DBG
void sendPairing() {																	// send the first pairing request
	hm.startPairing();
}
void resetDevice() {
	Serial << F("reset device, clear eeprom...\n");
	hm.reset();
	Serial << F("reset done\n");
}
void sendCmdStr() {																		// reads a sndStr from console and put it in the send queue
	memcpy(hm.send.data,parser.buffer,parser.count());									// take over the parsed byte data
	Serial << F("s: ") << pHexL(hm.send.data, hm.send.data[0]+1) << '\n';				// some debug string
	hm.send_out();																		// fire to send routine
}
void buttonSend() {
	uint8_t cnl, lpr;
	parser >> cnl >> lpr;
	
	Serial << "button press, cnl: " << cnl << ", long press: " << lpr << '\n';			// some debug message
	hm.sendPeerREMOTE(cnl,lpr,0);														// parameter: button/channel, long press, battery
}
void printConfig() {
	hm.printConfig();
}
#endif





