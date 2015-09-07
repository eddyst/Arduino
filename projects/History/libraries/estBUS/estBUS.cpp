/*
 estBUS protocol library
 by Eddy Steier
 Version 0.1
 
 Licence: Released for public use.
 
 see estBUS.h
 */


#include <estBUS.h>

// allocate the requested buffer size
void estBUS::begin ()
{
  inData_ = (byte *) malloc (bufferSize_);
  outData_ = (byte *) malloc (bufferSize_ + 4);
  reset ();
  errorCount_ = 0;
  Modus = reciving;
 
  noInterrupts();           // disable all interrupts
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A = 150;              // compare match register 16MHz/256/2Hz
  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2B |= (1 << CS21)              ; //    8 prescaler 
//TCCR2B |= (1 << CS21) | (1 << CS20); //   64 prescaler 
//TCCR2B |= (1 << CS22)              ; //  256 prescaler 
//TCCR2B |= (1 << CS22) | (1 << CS20); // 1024 prescaler 
// Wir starten den Timer erst später:  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
} // end of estBUS::begin 

// get rid of the buffer
void estBUS::stop ()
{
  reset ();
  free (inData_);
  inData_ = NULL;
  free (outData_);
  outData_ = NULL;
} // end of estBUS::stop 

// called after an error to return to "not in a packet"
void estBUS::reset ()
{
  haveSTX_ = false;
  available_ = false;
  inputPos_ = 0;
  startTime_ = 0;
} // end of estBUS::reset

// calculate 8-bit CRC
byte estBUS::crc8 (const byte *addr, byte len)
{
  byte crc = 0;
  while (len--) 
  {
    byte inbyte = *addr++;
    for (byte i = 8; i; i--)
    {
      byte mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) 
        crc ^= 0x8C;
      inbyte >>= 1;
    }  // end of for
  }  // end of while
  return crc;
}  // end of estBUS::crc8


// send a message of "length" bytes (max 128) to other end
// put length (with first bit have to be 0) at start, followed by data, and add CRC of data
boolean estBUS::sendMsg (const byte * data, const byte length)
{ // no callback? Can't send
	if (length > 128)
		return false;
  if (outLength_ > 0)  //nur wenn der outpuffer leer ist, können wir Daten annehmen
      return false;
 
	outData_[outLength_++] = length;  // STX
	for ( byte i = 0; i < length; i++) {
		outData_[outLength_++] = (data[i]);
	}
	outData_[outLength_++] = crc8 (data, length);
	outPos_    = 0;
	TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
	return true;
}  // end of estBUS::sendMsg

ISR(TIMER2_COMPA_vect)          // timer compare interrupt service routine
{ switch (outPosStep_++) {
  case 0: //Datenwert für aktuelles Bit setzen
		digitalWrite( txPin_, bitRead(outData_[ outPosByte_], outPosBit_));
		break;
  case 1: //Datenwert für aktuelles Bit prüfen
		if ( digitalRead( txPin_) != bitRead(outData_[ outPosByte_], outPosBit_)) { // Datenkollission - Senden abbrechen
			// Kann eigentlich nur beim rezessiven Bit auftreten!
		  errorCount_ ++;
			//ToDo: Wechsel in den Lesemodus
			outPosByte_ = 0;
			outPosBit_  = 0;
			outPosStep_ = 0;
		}
  case 0: //invertierten Datenwert für aktuelles Bit setzen
		digitalWrite( txPin_, ~bitRead(outData_[ outPosByte_], outPosBit_));
		break;
  }
  case 2: //Datenwert für aktuelles Bit prüfen
		if ( digitalRead( txPin_) != ~bitRead(outData_[ outPosByte_], outPosBit_)) { // Datenkollission - Senden abbrechen
			// Kann eigentlich nur beim rezessiven Bit auftreten!
		  errorCount_ ++;
			//ToDo: Wechsel in den Lesemodus
			outPosByte_ = 0;
			outPosBit_  = 0;
			outPosStep_ = 0;
		} else {
			if ( outPosBit_++ > 7) {
				if ( outPosByte_++ > outLength) {
					bitClear( TIMSK2, OCIE2A);  // disable timer compare interrupt
				}
			}
			outPosStep_ = 0;
		}
}




// called periodically from main loop to process data and 
// assemble the finished packet in 'inData_'

// returns true if packet received.

// You could implement a timeout by seeing if isPacketStarted() returns
// true, and if too much time has passed since getPacketStartTime() time.

bool estBUS::update ()
{
  // no data? can't go ahead (eg. begin() not called)
  if (inData_ == NULL)
    return false;

  if ( modus_ == sending && outLength > 0)
  {  

  fWriteCallback_ (what);

    sendByte (outData_[outPos]);
  }
  while (fAvailableCallback_ () > 0) 
  {
    byte inByte = fReadCallback_ ();

    switch (inByte)
    {

    case STX:   // start of text
      haveSTX_ = true;
      haveETX_ = false;
      inputPos_ = 0;
      firstNibble_ = true;
      startTime_ = millis ();
      break;

    case ETX:   // end of text (now expect the CRC check)
      haveETX_ = true;   
      break;

    default:
      // wait until packet officially starts
      if (!haveSTX_)
        break;   

      // check byte is in valid form (4 bits followed by 4 bits complemented)
      if ((inByte >> 4) != ((inByte & 0x0F) ^ 0x0F) )
      {
        reset ();
        errorCount_++;
        break;  // bad character
      } // end if bad byte 

      // convert back 
      inByte >>= 4;

      // high-order nibble?
      if (firstNibble_)
      {
        currentoutPosByte_ = inByte;
        firstNibble_ = false;
        break;
      }  // end of first nibble

      // low-order nibble
      currentoutPosByte_ <<= 4;
      currentoutPosByte_ |= inByte;
      firstNibble_ = true;

      // if we have the ETX this must be the CRC
      if (haveETX_)
      {
        if (crc8 (inData_, inputPos_) != currentoutPosByte_)
        {
          reset ();
          errorCount_++;
          break;  // bad crc  
        } // end of bad CRC

        available_ = true;
        return true;  // show data ready
      }  // end if have ETX already

      // keep adding if not full
      if (inputPos_ < bufferSize_)
        inData_ [inputPos_++] = currentoutPosByte_;
      else
      {
        reset (); // overflow, start again
        errorCount_++;
      }

      break;

    }  // end of switch
  }  // end of while incoming data 

  return false;  // not ready yet
} // end of estBUS::update


