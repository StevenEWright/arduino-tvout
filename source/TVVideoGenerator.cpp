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

#include <avr/interrupt.h>
#include <avr/io.h>

#include "TVAssemblyMacros.h"
#include "TVHardwareSetup.h"
#include "TVVideoGenerator.h"
#include "TVVideoTimings.h"

typedef struct TVVideoGenerator {
  volatile int scanLine;
  volatile unsigned long frames;
  unsigned char start_render;
  int lines_frame;      // remove me
  uint8_t vres;
  uint8_t hres;
  uint8_t output_delay; // remove me
  char vscale_const;    // combine me with status switch
  char vscale;          // combine me too.
  char vsync_end;       // remove me
  uint8_t *screen;
} TVVideoGenerator;

static TVVideoGenerator display;

static void (*render_line)(void);
static void (*line_handler)(void);

static void blank_line();
static void active_line();
static void vsync_line();

// 6cycles functions
static void render_line6c();
static void render_line5c();
static void render_line4c();
static void render_line3c();
static void inline wait_until(uint8_t time);

static int renderLine;

void TVVideoGeneratorBegin(uint8_t mode, uint8_t width, uint8_t height, uint8_t *screenBuffer) {
  display.screen = scrnptr;
  display.hres = x;
  display.vres = y;
  display.frames = 0;

  if (mode)
    display.vscale_const = _PAL_LINE_DISPLAY / display.vres - 1;
  else
    display.vscale_const = _NTSC_LINE_DISPLAY / display.vres - 1;

  display.vscale = display.vscale_const;

  // selects the widest render method that fits in 46us
  // as of 9/16/10 rendermode 3 will not work for resolutions lower than
  // 192(display.hres lower than 24)
  unsigned char rmethod = (_TIME_ACTIVE * _CYCLES_PER_US) / (display.hres * 8);
  switch (rmethod) {
  case 6:
    render_line = &render_line6c;
    break;
  case 5:
    render_line = &render_line5c;
    break;
  case 4:
    render_line = &render_line4c;
    break;
  case 3:
    render_line = &render_line3c;
    break;
  default:
    if (rmethod > 6)
      render_line = &render_line6c;
    else
      render_line = &render_line3c;
  }

  DDR_VID |= _BV(VID_PIN);
  DDR_SYNC |= _BV(SYNC_PIN);
  PORT_VID &= ~_BV(VID_PIN);
  PORT_SYNC |= _BV(SYNC_PIN);
  DDR_SND |= _BV(SND_PIN); // for tone generation.

  // inverted fast pwm mode on timer 1
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

  if (mode) {
    display.start_render =
        _PAL_LINE_MID - ((display.vres * (display.vscale_const + 1)) / 2);
    display.output_delay = _PAL_CYCLES_OUTPUT_START;
    display.vsync_end = _PAL_LINE_STOP_VSYNC;
    display.lines_frame = _PAL_LINE_FRAME;
    ICR1 = _PAL_CYCLES_SCANLINE;
    OCR1A = _CYCLES_HORZ_SYNC;
  } else {
    display.start_render =
        _NTSC_LINE_MID - ((display.vres * (display.vscale_const + 1)) / 2) + 8;
    display.output_delay = _NTSC_CYCLES_OUTPUT_START;
    display.vsync_end = _NTSC_LINE_STOP_VSYNC;
    display.lines_frame = _NTSC_LINE_FRAME;
    ICR1 = _NTSC_CYCLES_SCANLINE;
    OCR1A = _CYCLES_HORZ_SYNC;
  }
  display.scanLine = display.lines_frame + 1;
  line_handler = &vsync_line;
  TIMSK1 = _BV(TOIE1);
  sei();
}

// render a line
ISR(TIMER1_OVF_vect) {
  line_handler();
}

static void blank_line() {
  if (display.scanLine == display.start_render) {
    renderLine = 0;
    display.vscale = display.vscale_const;
    line_handler = &active_line;
  } else if (display.scanLine == display.lines_frame) {
    line_handler = &vsync_line;
  }

  display.scanLine++;
}

static void active_line() {
  wait_until(display.output_delay);
  render_line();
  if (!display.vscale) {
    display.vscale = display.vscale_const;
    renderLine += display.hres;
  } else
    display.vscale--;

  if ((display.scanLine + 1) ==
      (int)(display.start_render + (display.vres * (display.vscale_const + 1))))
    line_handler = &blank_line;

  display.scanLine++;
}

static void vsync_line() {
  if (display.scanLine >= display.lines_frame) {
    OCR1A = _CYCLES_VIRT_SYNC;
    display.scanLine = 0;
    display.frames++;

    if (remainingToneVsyncs != 0) {
      if (remainingToneVsyncs > 0) {
        remainingToneVsyncs--;
      }

    } else {
      TCCR2B = 0; // stop the tone
      PORTB &= ~(_BV(SND_PIN));
    }

  } else if (display.scanLine == display.vsync_end) {
    OCR1A = _CYCLES_HORZ_SYNC;
    line_handler = &blank_line;
  }
  display.scanLine++;
}

static void inline wait_until(uint8_t time) {
  __asm__ __volatile__("subi  %[time], 10\n"
                       "sub  %[time], %[tcnt1l]\n"
                       "100:\n"
                       "subi  %[time], 3\n"
                       "brcc  100b\n"
                       "subi  %[time], 0-3\n"
                       "breq  101f\n"
                       "dec  %[time]\n"
                       "breq  102f\n"
                       "rjmp  102f\n"
                       "101:\n"
                       "nop\n"
                       "102:\n"
                       :
                       : [time] "a"(time), [tcnt1l] "a"(TCNT1L));
}

static void render_line6c() {
  __asm__ __volatile__("ADD  r26,r28\n"
                       "ADC  r27,r29\n"
                       // save PORTB
                       "svprt  %[port]\n"

                       "rjmp  enter6\n"
                       "loop6:\n"
                       "bst  __tmp_reg__,0\n" // 8
                       "o1bs  %[port]\n"
                       "enter6:\n"
                       "LD    __tmp_reg__,X+\n" // 1
                       "delay1\n"
                       "bst  __tmp_reg__,7\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 2
                       "bst  __tmp_reg__,6\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 3
                       "bst  __tmp_reg__,5\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 4
                       "bst  __tmp_reg__,4\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 5
                       "bst  __tmp_reg__,3\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 6
                       "bst  __tmp_reg__,2\n"
                       "o1bs  %[port]\n"
                       "delay3\n" // 7
                       "bst  __tmp_reg__,1\n"
                       "o1bs  %[port]\n"
                       "dec  %[hres]\n"
                       "brne  loop6\n" // go too loopsix
                       "delay2\n"
                       "bst  __tmp_reg__,0\n" // 8
                       "o1bs  %[port]\n"

                       "svprt  %[port]\n" BST_HWS "o1bs  %[port]\n"
                       :
                       : [port] "i"(_SFR_IO_ADDR(PORT_VID)),
                         "x"(display.screen), "y"(renderLine),
                         [hres] "d"(display.hres)
                       : "r16" // try to remove this clobber later...
                       );
}

static void render_line5c() {
  __asm__ __volatile__("ADD  r26,r28\n"
                       "ADC  r27,r29\n"
                       // save PORTB
                       "svprt  %[port]\n"

                       "rjmp  enter5\n"
                       "loop5:\n"
                       "bst  __tmp_reg__,0\n" // 8
                       "o1bs  %[port]\n"
                       "enter5:\n"
                       "LD    __tmp_reg__,X+\n" // 1
                       "bst  __tmp_reg__,7\n"
                       "o1bs  %[port]\n"
                       "delay2\n" // 2
                       "bst  __tmp_reg__,6\n"
                       "o1bs  %[port]\n"
                       "delay2\n" // 3
                       "bst  __tmp_reg__,5\n"
                       "o1bs  %[port]\n"
                       "delay2\n" // 4
                       "bst  __tmp_reg__,4\n"
                       "o1bs  %[port]\n"
                       "delay2\n" // 5
                       "bst  __tmp_reg__,3\n"
                       "o1bs  %[port]\n"
                       "delay2\n" // 6
                       "bst  __tmp_reg__,2\n"
                       "o1bs  %[port]\n"
                       "delay1\n" // 7
                       "dec  %[hres]\n"
                       "bst  __tmp_reg__,1\n"
                       "o1bs  %[port]\n"
                       "brne  loop5\n" // go too loop5
                       "delay1\n"
                       "bst  __tmp_reg__,0\n" // 8
                       "o1bs  %[port]\n"

                       "svprt  %[port]\n" BST_HWS "o1bs  %[port]\n"
                       :
                       : [port] "i"(_SFR_IO_ADDR(PORT_VID)),
                         "x"(display.screen), "y"(renderLine),
                         [hres] "d"(display.hres)
                       : "r16" // try to remove this clobber later...
                       );
}

static void render_line4c() {
  __asm__ __volatile__("ADD  r26,r28\n"
                       "ADC  r27,r29\n"

                       "rjmp  enter4\n"
                       "loop4:\n"
                       "lsl  __tmp_reg__\n" // 8
                       "out  %[port],__tmp_reg__\n"
                       "enter4:\n"
                       "LD    __tmp_reg__,X+\n" // 1
                       "delay1\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay2\n" // 2
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay2\n" // 3
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay2\n" // 4
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay2\n" // 5
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay2\n" // 6
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay1\n" // 7
                       "lsl  __tmp_reg__\n"
                       "dec  %[hres]\n"
                       "out  %[port],__tmp_reg__\n"
                       "brne  loop4\n" // go too loop4
                       "delay1\n"        // 8
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n"
                       "delay3\n"
                       "cbi  %[port],7\n"
                       :
                       : [port] "i"(_SFR_IO_ADDR(PORT_VID)),
                         "x"(display.screen), "y"(renderLine),
                         [hres] "d"(display.hres)
                       : "r16" // try to remove this clobber later...
                       );
}

/** @remarks only 16mhz right now! */
static void render_line3c() {
  __asm__ __volatile__(".macro byteshift\n"
                       "LD    __tmp_reg__,X+\n"
                       "out  %[port],__tmp_reg__\n" // 0
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 1
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 2
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 3
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 4
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 5
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 6
                       "nop\n"
                       "lsl  __tmp_reg__\n"
                       "out  %[port],__tmp_reg__\n" // 7
                       ".endm\n"

                       "ADD  r26,r28\n"
                       "ADC  r27,r29\n"

                       "cpi  %[hres],30\n" // 615
                       "breq  skip0\n"
                       "cpi  %[hres],29\n"
                       "breq  jumpto1\n"
                       "cpi  %[hres],28\n"
                       "breq  jumpto2\n"
                       "cpi  %[hres],27\n"
                       "breq  jumpto3\n"
                       "cpi  %[hres],26\n"
                       "breq  jumpto4\n"
                       "cpi  %[hres],25\n"
                       "breq  jumpto5\n"
                       "cpi  %[hres],24\n"
                       "breq  jumpto6\n"
                       "jumpto1:\n"
                       "rjmp  skip1\n"
                       "jumpto2:\n"
                       "rjmp  skip2\n"
                       "jumpto3:\n"
                       "rjmp  skip3\n"
                       "jumpto4:\n"
                       "rjmp  skip4\n"
                       "jumpto5:\n"
                       "rjmp  skip5\n"
                       "jumpto6:\n"
                       "rjmp  skip6\n"
                       "skip0:\n"
                       "byteshift\n" // 1    \\643
                       "skip1:\n"
                       "byteshift\n" // 2
                       "skip2:\n"
                       "byteshift\n" // 3
                       "skip3:\n"
                       "byteshift\n" // 4
                       "skip4:\n"
                       "byteshift\n" // 5
                       "skip5:\n"
                       "byteshift\n" // 6
                       "skip6:\n"
                       "byteshift\n" // 7
                       "byteshift\n" // 8
                       "byteshift\n" // 9
                       "byteshift\n" // 10
                       "byteshift\n" // 11
                       "byteshift\n" // 12
                       "byteshift\n" // 13
                       "byteshift\n" // 14
                       "byteshift\n" // 15
                       "byteshift\n" // 16
                       "byteshift\n" // 17
                       "byteshift\n" // 18
                       "byteshift\n" // 19
                       "byteshift\n" // 20
                       "byteshift\n" // 21
                       "byteshift\n" // 22
                       "byteshift\n" // 23
                       "byteshift\n" // 24
                       "byteshift\n" // 25
                       "byteshift\n" // 26
                       "byteshift\n" // 27
                       "byteshift\n" // 28
                       "byteshift\n" // 29
                       "byteshift\n" // 30

                       "delay2\n"
                       "cbi  %[port],7\n"
                       :
                       : [port] "i"(_SFR_IO_ADDR(PORT_VID)),
                         "x"(display.screen), "y"(renderLine),
                         [hres] "d"(display.hres)
                       : "r16" // try to remove this clobber later...
                       );
}
