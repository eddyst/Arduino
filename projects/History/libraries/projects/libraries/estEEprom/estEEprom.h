/*
  EEPROM.h - EEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef estEEprom_h
#define estEEprom_h

#include <inttypes.h>

class estEEpromClass
{
  public:
    uint8_t  readByte(int);
		uint16_t readWord(int);
    void writeByte(int, uint8_t );
		void writeWord(int, uint16_t);
    void updateByte(int, uint8_t );
    void updateWord(int, uint16_t);
};

extern estEEpromClass EEPROM;

#endif

