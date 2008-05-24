// ### BOILERPLATE ###
// Alternate Pace Firmware
// Copyright (C) 2007 Peter Todd <pete@petertodd.org>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ### BOILERPLATE ###

#include <common.h>
#include <loop.h>
#include <display.h>

void main_loop(){
  while (1){
    // Insert seti@home implementation here.
#ifdef TEST_DISPLAY
    uint8_t i;
    for (i = 0; i <= CHAR_BLANK; i++){
      display_digits_test(i % (CHAR_BLANK + 1),(i+1) % (CHAR_BLANK + 1),(i+2) % (CHAR_BLANK + 1),(i+3) % (CHAR_BLANK + 1),(i+4) % (CHAR_BLANK + 1),(i+5) % (CHAR_BLANK + 1));
      delay100ktcy(1);
    }
#endif
  }
}

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:
