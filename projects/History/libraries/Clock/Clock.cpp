/* *********************************************************************
   Simple class that provides the current time & date. At intervals,
   the network time protocol (NTP) is used to obtain the time from
   a time server. Between synchronization, the arduino's millisecond
   counter is used to extrapolate the current time. 
   
   NTP query based on code from Arduino Ethernet library.
   Conversion between Unix time & natural time represenation based on 
   code by Michael Margolis. 
   
   This library is free software; you can redistribute it and/or 
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   Visit http://www.MegunoLink.com/Libraries for the latest version.
   ********************************************************************* */

#include "clock.h"
EthernetUDP UdpConnection;

Clock::Clock() : 
  mc_nLocalUDPPort(8880), 
  //mc_ulRefreshInterval(5UL * 60UL * 60UL * 1000UL), // normally every 5 hours
  mc_ulRefreshInterval(20 * 1000UL), // every 20 seconds for debuggin
  mc_ulRetryInterval(5 * 1000UL),
  mc_TimeServer(192,53,103,108) // ptbtime3.ptb.de
{
  // Force refresh of NTP time in a second. 
  m_ulLastAcquired = millis() - (mc_ulRefreshInterval + 1000);
  m_ulLastNTPTime = 0;
  m_ulLastAttempt = 0;
  
  // Assume zero offset for timezone.
  m_nTimezoneOffset = 0;
}

/*
void Clock::Setup(HardwareSerial *pPort){
	Debug = pPort;
}
*/

void Clock::Maintain()
{
  unsigned long ulCurrent = millis();
  static boolean m_NTPReciveing = false;
  if (!m_NTPReciveing) {
//    #ifdef DEBUG
//	  DEBUG.println( "m_NTPReciveing=false");
//    #endif
    // Update NTP time if we haven't acquired it for a while &&
    // it has been a reasonable time since the last attempt.
    if (ulCurrent - m_ulLastAcquired > mc_ulRefreshInterval &&
       (m_ulLastAttempt == 0 || ulCurrent - m_ulLastAttempt > mc_ulRetryInterval)){
      #ifdef DEBUG
		DEBUG.print( "Refreshing NTPTime - ulCurrent=");
		DEBUG.print(ulCurrent);
		DEBUG.print( ", m_ulLastAcquired=");
		DEBUG.print(m_ulLastAcquired);
		DEBUG.print( ", mc_ulRefreshInterval=");
		DEBUG.print(mc_ulRefreshInterval);
		DEBUG.print( ", m_ulLastAttempt=");
		DEBUG.println(m_ulLastAttempt);
      #endif
      UdpConnection.begin(mc_nLocalUDPPort);
      SendNTPPacket(UdpConnection, mc_TimeServer);
	  m_ulLastAttempt = ulCurrent; 
	  m_NTPReciveing = true;
      #ifdef DEBUG
		DEBUG.print( "m_ulLastAttempt=");
		DEBUG.println(m_ulLastAttempt);
      #endif
	}
  }
  else {
    #ifdef DEBUG
 	  DEBUG.print( "NTPReciveing - ulCurrent=");
      DEBUG.print(ulCurrent);
      DEBUG.print(", m_ulLastAttempt=");
	  DEBUG.print(m_ulLastAttempt);
      DEBUG.print("B");
    #endif
	int x = UdpConnection.parsePacket();
    #ifdef DEBUG
      DEBUG.print(", Count=");
	  DEBUG.println(x);
    #endif
   if (x<46) {
	  if (ulCurrent - m_ulLastAttempt > 5000) {
	    #ifdef DEBUG
		  DEBUG.println("NTPTime Timeout, no-response received");
	    #endif
	    m_ulLastAttempt = ulCurrent; // Will retry a little later. 
        m_NTPReciveing = false;
	    UdpConnection.stop();
	  }
	}
    else {
	  int nByte;
	  #ifdef DEBUG
		DEBUG.println("NTPTime response received");
	  #endif
	  // Dump the first 40 bytes. 
	  for( nByte = 0; nByte < 40; ++nByte)
	    UdpConnection.read();
	  // next four bytes are 64 bit integer time stamp of
	  // seconds since 1 Jan 1900. 
	  unsigned long ul1900Seconds = 0;
	  for(nByte = 0; nByte < 4; ++nByte)
	    ul1900Seconds = (ul1900Seconds<<8) | UdpConnection.read();
	  // Convert from NTP time basis to Unix time basis (seconds
	  // since 1 Jan 1970) & save. Record the machine time when
	  // this time was recorded to extrapolate the current time.
	  m_ulLastNTPTime = ul1900Seconds - 2208988800UL;
	  m_ulLastAcquired = ulCurrent;
	  m_ulLastAttempt = 0; // success.
      m_NTPReciveing = false;
	  UdpConnection.stop();
    }
  }
}

unsigned long Clock::GetTimestamp()
{
  unsigned long ulCurrentOffset;
  if (m_ulLastNTPTime == 0)
    return 0;
  ulCurrentOffset = millis() - m_ulLastAcquired;
  ulCurrentOffset = (ulCurrentOffset + 500) / 1000; // Round to nearest second.
  ulCurrentOffset += m_nTimezoneOffset * 60L; // adjust for timezone [mins].
  return ulCurrentOffset + m_ulLastNTPTime;
}
/*
void Clock::WriteTime(HardwareSerial *pPort)
{
  unsigned long ulTimestamp;
  
  ulTimestamp = GetTimestamp();
  
  pPort->print((ulTimestamp % 86400L)/3600); // Print hour
  pPort->print(':');
  if ( ((ulTimestamp % 3600) / 60) < 10 )  // print leading 0 for first 10 min of each hour
    pPort->print('0');
  pPort->print((ulTimestamp  % 3600) / 60); // print the minute (3600 equals secs per minute)
  pPort->print(':'); 
  if ( (ulTimestamp % 60) < 10 ) // print leading 0 for first 10 sec of each min.
    pPort->print('0');
  pPort->print(ulTimestamp % 60); // print the second
}


void Clock::WriteDateTime(HardwareSerial *pPort)
{
  DateTime dt;
  
  GetDateTime(dt);
  pPort->print(dt.Year);
  pPort->print('/');
  if (dt.Month < 10)
    pPort->print('0');
  pPort->print(dt.Month);
  pPort->print('/');
  if (dt.Day < 10)
    pPort->print('0');
  pPort->print(dt.Day);
  pPort->print(' ');
  if (dt.Hour < 10)
    pPort->print('0');
  pPort->print(dt.Hour);
  pPort->print(':');
  if (dt.Minute < 10)
    pPort->print('0');
  pPort->print(dt.Minute);
  pPort->print(':');
  if (dt.Second < 10)
    pPort->print('0');
  pPort->print(dt.Second);
}

void Clock::DecodeTo(DateTime &dt)
{  
  unsigned long ulCurrent, ulDays;
  byte abyDaysPerMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  
  ulCurrent = GetTimestamp(); // Seconds since 1 Jan 1970
  
  dt.Second = ulCurrent % 60;
  ulCurrent /= 60; // now minutes since 1 Jan 1970
  dt.Minute = ulCurrent % 60; 
  ulCurrent /= 60; // hours
  dt.Hour = ulCurrent % 24;
  ulCurrent /= 24; // now days.
  
  // Determine year by counting days, taking care of leap years. 
  dt.Year = 1970;
  ulDays = 0;
  for(ulDays = 0, dt.Year = 1970; ulDays <= ulCurrent; ++dt.Year)
    ulDays += IsLeapYear(dt.Year) ? 366 : 365;
  --dt.Year; // we went one too far.
  
  ulDays -= IsLeapYear(dt.Year) ? 366 : 365;
  ulCurrent -= ulDays; // Now days this year, starting from 0.
  
  // Make an adjustment for feb, if this is a leap year, then figure
  // out which month we are in. 
  if (IsLeapYear(dt.Year))
    abyDaysPerMonth[1] = 29;
  
  for(dt.Month = 0, ulDays = 0; dt.Month < 12 && ulDays <= ulCurrent; ++dt.Month)
    ulDays += abyDaysPerMonth[dt.Month];
  dt.Day = ulCurrent - (ulDays - abyDaysPerMonth[dt.Month]);
}
*/

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

DateTime Clock::GetDateTime(){
	return GetDateTime( GetTimestamp());
}
DateTime Clock::GetDateTime(unsigned long time){
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!
  DateTime dt;
  uint8_t year;
  uint8_t month, monthLength;
  unsigned long days;
  
  dt.Second = time % 60;
  time /= 60; // now it is minutes
  dt.Minute = time % 60;
  time /= 60; // now it is hours
  dt.Hour = time % 24;
  time /= 24; // now it is days
  //dt.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  dt.Year = year + 1970; // year is offset from 1970 
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  dt.Month = month + 1;  // jan is month 1  
  dt.Day = time + 1;     // day of month
  return dt;
}


void Clock::SetTimezoneOffset(int nHours, int nMinutes)
{
  m_nTimezoneOffset = nHours * 60 + nMinutes;
}

void Clock::SendNTPPacket(EthernetUDP &rUDP, const IPAddress &rAddress)
{
  const int cnNTPPacketSize = 48;
  static byte abyPacketBuffer[cnNTPPacketSize] PROGMEM = 
    {   0b11100011,   // LI, Version, Mode
        0,     // Stratum, or type of clock
        6,     // Polling Interval
        0xEC,  // Peer Clock Precision
        0, 0, 0, 0, 0, 0, 0, 0,  // 8 bytes of zero for Root Delay & Root Dispersion
        49, 
        0x4E,
        49,
        52,
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
    };
  
  byte byValue, iByte;

  rUDP.beginPacket(rAddress, 123);
  for (iByte = 0; iByte < cnNTPPacketSize; ++iByte)
  {
    byValue = pgm_read_byte_near(abyPacketBuffer+iByte);
    rUDP.write(byValue);
  }
  rUDP.endPacket();
}

