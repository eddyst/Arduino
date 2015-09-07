/*
 estBUS protocol library
 inherits from:
		RS485 protocol library - non-blocking.
		Devised and written by Nick Gammon.
		Date: 4 December 2012
		Version: 1.0
 this is
 by Eddy Steier
 Version 0.1
 
 Licence: Released for public use.

 Simple Protokoll to use CAN-drivers as hardware.
 This give's the ability to detect collisons on the bus.
 
Can send from 1 to 128 bytes from one node to all others.
Each bit is doubled and inverted. This is to check validity 
and to prevent the timeout of the dominant level ( 400 ms for the chip I use)
 * Packet length (8 bit of whitch the first one has to be 0)
 * data
 * Packet CRC ( 8 bit checksum)

Example is:
	
estBUS myChannel ( 20); // bufferLength

void setup {
  myChannel.begin();
}	

void loop {
	myChannel.update();
}
*/

#include "Arduino.h"


class estBUS 
  {

  // where we save incoming stuff
  volatile uint8_t * inData_;
	
  // where we save outgoing stuff
  volatile uint8_t * outData_;
  volatile uint8_t outLength_;
  volatile uint8_t outPosByte_;
	volatile uint8_t outPosBit_;
	volatile uint8_t outPosStep_;

	
  // how much data is in the buffer
  const int bufferSize_;

  // this is true once we have valid data in buf
  bool available_;
  
  // an STX (start of text) signals a packet start
  bool haveSTX_;
  
  // count of errors
  unsigned long errorCount_;

//	enmModus modus_;
  
  // variables below are set when we get an STX
  bool haveETX_;
  byte inputPos_;
  byte currentByte_;
  bool firstNibble_;
  unsigned long startTime_;
	
	
  // helper private functions
  byte crc8 (const byte *addr, byte len);

		// returns true if packet available
  void sendComplemented (const byte what);
  void sendByte (const byte what);
	uint8_t rxPin_, txPin_;
	
  public:
    
    // constructor
    estBUS (const uint8_t rxPin, const uint8_t txPin, const byte bufferSize) :
				rxPin_ (rxPin),
				txPin_ (txPin),
        bufferSize_ (bufferSize),
        inData_ (NULL),
        outData_ (NULL) {}
  
    // destructor - frees memory used
    ~estBUS () { stop (); }
  
    // allocate memory for buf_
    void begin ();
    
    // free memory in buf_
    void stop ();
		
    // handle incoming data, return true if packet ready
    bool update ();
  
    // reset to no incoming data (eg. after a timeout)
    void reset ();

		// send data
		boolean sendMsg (const byte * data, const byte length);
  
		bool available () const { return available_; };
    // once available, returns the address of the current message
    const uint8_t * getData ()   const { return inData_; }
    const uint8_t   getLength () const { return inputPos_; }
    
    // return how many errors we have had
    unsigned long getErrorCount () const { return errorCount_; }
  
    // return when last packet started
    unsigned long getPacketStartTime () const { return startTime_; }
  
    // return true if a packet has started to be received
    bool isPacketStarted () const { return haveSTX_; }
  
  }; // end of class estBUS

