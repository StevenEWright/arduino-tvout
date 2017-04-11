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

#include "TVHardwareSetup.h"

#ifndef TVAssemblyMacros_H
#define TVAssemblyMacros_H

// delay macros
__asm__ __volatile__ (
  // delay 1 clock cycle.
  ".macro delay1\n\t"
    "nop\n"
  ".endm\n"

  // delay 2 clock cycles
  ".macro delay2\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 3 clock cyles
  ".macro delay3\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 4 clock cylces
  ".macro delay4\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 5 clock cylces
  ".macro delay5\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 6 clock cylces
  ".macro delay6\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 7 clock cylces
  ".macro delay7\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 8 clock cylces
  ".macro delay8\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 9 clock cylces
  ".macro delay9\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"

  // delay 10 clock cylces
  ".macro delay10\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n\t"
    "nop\n"
  ".endm\n"
); // end of delay macros

// common output macros, specific output macros at top of file
__asm__ __volatile__ (

  // save port 16 and clear the video bit
  ".macro svprt p\n\t"
    "in    r16,\\p\n\t"
    ANDI_HWS
  ".endm\n"

  // ouput 1 bit port safe
  ".macro o1bs p\n\t"
    BLD_HWS
    "out  \\p,r16\n"
  ".endm\n"
); // end of output macros

#endif  /* TVAssemblyMacros_H */
