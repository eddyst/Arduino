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
  data_ = (byte *) malloc (bufferSize_);
  out_ = (byte *) malloc (bufferSize_ + 4);
  reset ();
  errorCount_ = 0;
  Modus = reciving;

} // end of estBUS::begin 

// get rid of the buffer
void estBUS::stop ()
{
  reset ();
  free (data_);
  data_ = NULL;
  free (out_);
  out_ = NULL;
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


// send a message of "length" bytes (max 255) to other end
// put STX at start, ETX at end, and add CRC
boolean estBUS::sendMsg (const byte * data, const byte length)
{ // no callback? Can't send
	if (fWriteCallback_ != NULL &&)
		return false;

  if (outLength_ == 0) { //nur wenn der outpuffer leer ist, können wir Daten annehmen
    byte c;
    out_[outLength_++] = STX;  // STX
		// send a byte complemented, repeated
		// only values sent would be (in hex): 
		//   0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0
    for ( byte i = 0; i < length; i++) {
      // first nibble
      c = data[i] >> 4;
      out_[outLength_++] = (c << 4) | (c ^ 0x0F); 
      // second nibble
      c = data[i] & 0x0F;
      out_[outLength_++] = (c << 4) | (c ^ 0x0F);
    }
    out_[outLength_++] = ETX;  // ETX
		byte crc = crc8 (data, length);
      c = crc >> 4;
      out_[outLength_++] = (c << 4) | (c ^ 0x0F); 
      // second nibble
      c = crc & 0x0F;
      out_[outLength_++] = (c << 4) | (c ^ 0x0F);
    outPos_    = 0;
    modus_ = sending;
    return true;
  } 
  else { 
    return false;
  }
}  // end of estBUS::sendMsg


boolean estBUS::sendRequest (const byte id)
{ if (fWriteCallback_ != NULL &&)
		return false;
  if (outLength_ == 0) { //nur wenn der outpuffer leer ist, können wir Daten annehmen
    byte c;
    out_[outLength_++] = RTX;  // STX
		// send a byte complemented, repeated
		// only values sent would be (in hex): 
		//   0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0

		// first nibble
    c = id >> 4;
    out_[outLength_++] = (c << 4) | (c ^ 0x0F); 
    // second nibble
    c = id & 0x0F;
    out_[outLength_++] = (c << 4) | (c ^ 0x0F);
    outPos_    = 0;
    modus_ = sending;
    return true;
  } 
  else { 
    return false;
  }
}  // end of estBUS::sendMsg

void estBUS::sendByte (const byte what) {
  // no callback? Can't send
  if (fWriteCallback_ == NULL)
    return;

  fWriteCallback_ (what);
}  // end of estBUS::sendByte


// called periodically from main loop to process data and 
// assemble the finished packet in 'data_'

// returns true if packet received.

// You could implement a timeout by seeing if isPacketStarted() returns
// true, and if too much time has passed since getPacketStartTime() time.

bool estBUS::update ()
{
  // no data? can't go ahead (eg. begin() not called)
  if (data_ == NULL)
    return false;

  // no callbacks? Can't read
  if (fAvailableCallback_ == NULL || fReadCallback_ == NULL)
    return false;

  if ( modus_ == sending && outLength > 0)
  {  

  fWriteCallback_ (what);

    sendByte (out_[outPos]);
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
        currentByte_ = inByte;
        firstNibble_ = false;
        break;
      }  // end of first nibble

      // low-order nibble
      currentByte_ <<= 4;
      currentByte_ |= inByte;
      firstNibble_ = true;

      // if we have the ETX this must be the CRC
      if (haveETX_)
      {
        if (crc8 (data_, inputPos_) != currentByte_)
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
        data_ [inputPos_++] = currentByte_;
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


