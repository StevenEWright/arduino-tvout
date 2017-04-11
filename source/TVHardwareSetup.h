/*
  Copyright (c) 2017 Steven E Wright

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Copyright (c) 2010 Myles Metzer

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

// sound is output on OC2A
// sync output is on OC1A

#ifndef TVHardwareSetup_H
#define TVHardwareSetup_H

// device specific settings.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
//video
#define PORT_VID  PORTA
#define  DDR_VID    DDRA
#define VID_PIN    7
//sync
#define PORT_SYNC  PORTB
#define DDR_SYNC  DDRB
#define  SYNC_PIN  5

#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
//video
#define PORT_VID  PORTA
#define  DDR_VID    DDRA
#define VID_PIN    7
//sync
#define PORT_SYNC  PORTD
#define DDR_SYNC  DDRD
#define SYNC_PIN  5

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
//video
#define PORT_VID  PORTD
#define  DDR_VID    DDRD
#define  VID_PIN    7
//sync
#define PORT_SYNC  PORTB
#define DDR_SYNC  DDRB
#define SYNC_PIN  1

#elif defined (__AVR_AT90USB1286__)
//video
#define PORT_VID  PORTF
#define  DDR_VID    DDRF
#define  VID_PIN    7
//sync
#define PORT_SYNC  PORTB
#define DDR_SYNC  DDRB
#define SYNC_PIN  5

#elif defined(__AVR_ATmega32U4__) // Modified for Arduino Leonardo
//video
#define PORT_VID  PORTB
#define  DDR_VID    DDRB
#define  VID_PIN    4 // 8
//sync
#define PORT_SYNC  PORTB
#define DDR_SYNC  DDRB
#define SYNC_PIN  5 // 9

#define TCCR2A    TCCR0A
#define TCCR2B    TCCR0B
#define OCR2A    OCR0A
#define OCR2B    OCR0B
#define COM2A0    COM0A0
#define COM2A1    COM0A1
#define CS20    CS00
#define WGM21    WGM01
#endif

//automatic BST/BLD/ANDI macro definition
#if VID_PIN == 0
  #define BLD_HWS    "bld  r16,0\n"
  #define BST_HWS    "bst  r16,0\n"
  #define ANDI_HWS   "andi  r16,0xFE\n"
#elif VID_PIN == 1
  #define BLD_HWS    "bld  r16,1\n"
  #define BST_HWS    "bst  r16,1\n"
  #define ANDI_HWS   "andi  r16,0xFD\n"
#elif VID_PIN == 2
  #define BLD_HWS    "bld  r16,2\n"
  #define BST_HWS    "bst  r16,2\n"
  #define ANDI_HWS   "andi  r16,0xFB\n"
#elif VID_PIN == 3
  #define BLD_HWS    "bld  r16,3\n"
  #define BST_HWS    "bst  r16,3\n"
  #define ANDI_HWS   "andi  r16,0xF7\n"
#elif VID_PIN == 4
  #define BLD_HWS    "bld  r16,4\n"
  #define BST_HWS    "bst  r16,4\n"
  #define ANDI_HWS   "andi  r16,0xEF\n"
#elif VID_PIN == 5
  #define BLD_HWS    "bld  r16,5\n"
  #define BST_HWS    "bst  r16,5\n"
  #define ANDI_HWS   "andi  r16,0xDF\n"
#elif VID_PIN == 6
  #define BLD_HWS    "bld  r16,6\n"
  #define BST_HWS    "bst  r16,6\n"
  #define ANDI_HWS   "andi  r16,0xBF\n"
#elif VID_PIN == 7
  #define BLD_HWS    "bld  r16,7\n"
  #define BST_HWS    "bst  r16,7\n"
  #define ANDI_HWS   "andi  r16,0x7F\n"
#endif

#endif  /* TVHardwareSetup_H */
