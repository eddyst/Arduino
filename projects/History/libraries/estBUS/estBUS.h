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

 Improves the RS485 library by using CAN-drivers as hardware.
 This give's the ability to detect collisons on the bus.
 
 Can send from 1 to 255 bytes from one node to another with:
 
 * Packet start indicator (STX)
 * Each data byte is doubled and inverted to check validity
 * Packet end indicator (ETX)
 * Packet CRC (checksum)

 To allow flexibility with hardware (eg. Serial, SoftwareSerial, I2C)
 you provide three "callback" functions which send or receive data. Examples are:
 
  size_t fWrite( const byte what) {
    return Serial.write( what);  
  }
 
  int fAvailable() {
    return Serial.available();  
  }
 
  int fRead() {
    return Serial.read();  
  }
	 
  void fPrepareAnswer() {
	  // Fill 
	}

  void fMessageRecived() { 
		// Fill 
	}
	
estBUS myChannel (fRead, fAvailable, fWrite, fPrepareAnswer, fMessageRecived, 20);

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

  typedef size_t ( *WriteCallback         ) ( const byte what                                   ); // send a byte to serial port
  typedef int    ( *AvailableCallback     ) (                                                   ); // return number of bytes available
  typedef int    ( *ReadCallback          ) (                                                   ); // read a byte from serial port

	typedef int    ( *PrepareAnswerCallback ) ( const byte TimeSlotID, byte* data, byte dataLength); //ToDo: soll den Sketch nach den Antwortdaten für TimeSlotID fragen
	typedef int    ( *MessageRecivedCallback) ( const byte TimeSlotID, byte* data, byte dataLength); //Informiert den Sketch über neue Messages für TimeSlotID
	
  enum {RTX = '\1',   // Request of text = Timeslot for MessageID
        STX = '\2',   // start of text
        ETX = '\3'    // end of text
  };  // end of enum
  enum enmModus{
        sending,
        reciving
  } ;  // end of enum

  // callback functions to do reading/writing
  ReadCallback fReadCallback_;
  AvailableCallback fAvailableCallback_;
  WriteCallback fWriteCallback_; 
  PrepareAnswerCallback fPrepareAnswerCallback_;
	MessageRecivedCallback fMessageRecivedCallback_;
	
  // where we save incoming/outgoing stuff
  byte * data_;
  byte * out_;
	
  // how much data is in the buffer
  const int bufferSize_;

  // this is true once we have valid data in buf
  bool available_;
  
  // an STX (start of text) signals a packet start
  bool haveSTX_;
  
  // count of errors
  unsigned long errorCount_;

	enmModus modus_;
  
  // variables below are set when we get an STX
  bool haveETX_;
  byte inputPos_;
  byte currentByte_;
  bool firstNibble_;
  unsigned long startTime_;
	
  byte outLength_;
  byte outPos_;
	
  // helper private functions
  byte crc8 (const byte *addr, byte len);

	// send Request (Opens a Timeslot for Clients to Send their Data)
	boolean estBUS::sendRequest (const byte id)
	// send data
	boolean sendMsg (const byte * data, const byte length);
		// returns true if packet available
  void sendComplemented (const byte what);
  void sendByte (const byte what);
	
  public:
    
    // constructor
    estBUS (ReadCallback fReadCallback, 
           AvailableCallback fAvailableCallback, 
           WriteCallback fWriteCallback,
					 PrepareAnswerCallback fPrepareAnswerCallback,
					 MessageRecivedCallback fMessageRecivedCallback,
           const byte bufferSize) :
        fReadCallback_ (fReadCallback), 
        fAvailableCallback_ (fAvailableCallback), 
        fWriteCallback_ (fWriteCallback),
				fPrepareAnswerCallback_ (fPrepareAnswerCallback),
				fMessageRecivedCallback_ (fMessageRecivedCallback),
        bufferSize_ (bufferSize),
        data_ (NULL) {}
  
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

  
		bool available () const { return available_; };
    // once available, returns the address of the current message
    const byte * getData ()   const { return data_; }
    const byte   getLength () const { return inputPos_; }
    
    // return how many errors we have had
    unsigned long getErrorCount () const { return errorCount_; }
  
    // return when last packet started
    unsigned long getPacketStartTime () const { return startTime_; }
  
    // return true if a packet has started to be received
    bool isPacketStarted () const { return haveSTX_; }
  
  }; // end of class estBUS

