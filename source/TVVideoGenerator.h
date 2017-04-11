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
*/

#ifndef TVVideoGenerator_H
#define TVVideoGenerator_H

/** Specifies the PAL video format. */
static const uint8_t TVFormatPAL = 1;

/** Specifies the NTSC video format. */
static const uint8_t TVFormatNTSC = 0;

/** Begins generating the TV signal.
    @param mode One of: @c TVFormatPAL or TVFormatNTSC is expected.
    @param width Width of the screen in pixels. The default is 128.
    @param height Height of the screen in pixels. The default is 96.
    @param screenBuffer The screen buffer which contains monochrome pixel data aligned to each line
        on single-byte boundaries. */
void TVVideoGeneratorBegin(uint8_t mode, uint8_t width, uint8_t height, uint8_t *screenBuffer);

#endif  /* TVVideoGenerator_H */
