// ### BOILERPLATE ###
// Alternate Pace Firmware
// Copyright (C) 2007,2008 Peter Todd <pete@petertodd.org>
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
#include <display.h>

// Table with character to led mapping.
const uint8_t char_to_digit[] = {
  // ABCDEFGP
   b(11111100), // 0
   b(01100000), // 1
   b(11011010), // 2
   b(11110010), // 3
   b(01100110), // 4
   b(10110110), // 5
   b(10111110), // 6
   b(11100000), // 7
   b(11111110), // 8
   b(11100110), // 9
   b(11101110), // A
   b(00111110), // B
   b(10011100), // C
   b(01111010), // D
   b(10011110), // E
   b(10001110), // F
   b(00000000), // blank
   b(00000001)  // dot
};

void init_display(){
  TRISAbits.TRISA1 = 1;
  TRISAbits.TRISA2 = 0;
  TRISAbits.TRISA3 = 0;

  TRISBbits.TRISB3 = 0;
  TRISBbits.TRISB4 = 0;

  //display_digits(CHAR_BLANK,CHAR_BLANK,CHAR_BLANK,CHAR_BLANK,CHAR_BLANK,CHAR_BLANK);
  display_digits(0,1,2,3,4,5);
}


#define output_digit_bit(i) \
  if (w.b ## i){ \
    LATAbits.LATA2 = 1; \
  } else { \
    LATAbits.LATA2 = 0; \
  } \
  LATAbits.LATA3 = 1; \
  LATAbits.LATA3 = 0;

void output_digit(uint8_t d){
  bits_t w;
  
  w.b = d;

  output_digit_bit(0);
  output_digit_bit(1);
  output_digit_bit(2);
  output_digit_bit(3);
  output_digit_bit(4);
  output_digit_bit(5);
  output_digit_bit(6);
  output_digit_bit(7);
}

void output_flipped_digit(uint8_t d){
  output_digit(
               ((d & b(00011100)) << 3) |
               ((d & b(11100000)) >> 3) |
               ((d & b(00000011))));
}

uint8_t dot1 = b(00000001);

void display_digits_real(uint8_t d0,uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4,uint8_t d5){
  // Output on
  LATBbits.LATB3 = 0;

  // Write out the digit bit streams.
  output_flipped_digit(char_to_digit[d5]);
  output_flipped_digit(char_to_digit[d4] | b(00000001)); 
  output_digit(char_to_digit[d3] | b(00000001));
  output_flipped_digit(char_to_digit[d2] | b(00000001));
  output_digit(char_to_digit[d1] | dot1);
  output_digit(char_to_digit[d0]);

  // Clock the latch to put the data onto the display
  LATBbits.LATB4 = 1;
  LATBbits.LATB4 = 0;
}

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:
