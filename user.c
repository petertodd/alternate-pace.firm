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
#include <user.h>
#include <time.h>
#include <display.h>
#include <eeprom.h>
#include <debounce.h>
#include <metrics.h>
#include <ds3231.h>

enum mode_t {extra_fast_clock,fast_clock,normal_clock,slow_clock,extra_slow_clock,none_chosen};

enum mode_t mode;

bool silly_hour_display;

// Raw pin io's, true if pressed, false if not.
#define hours_switch_raw (!PORTBbits.RB2)
#define mins_switch_raw (!PORTAbits.RA6)

// SDCC is missing RA7...
#if 0
#define secs_switch_raw (!PORTAbits.RA7)
#else
#define secs_switch_raw (!(PORTA & b(10000000)))
#endif

static debounce_state_t hours_debounce,mins_debounce,secs_debounce;

void init_user(){
  uint8_t i;

  init_display();

  TRISBbits.TRISB2 = 1;

  // SDCC is missing the TRISA7 bit in the define...
#if 0
  TRISAbits.TRISA0 = 1;
  TRISAbits.TRISA6 = 1;
  TRISAbits.TRISA7 = 1;
#else
  TRISA = TRISA | b(11010001);
#endif

  // Setup pwm and adc for the led brightness control
  ADCON0 = b(00000001);
  ADCON1 = b(01111110);
  ADCON2 = b(00000000);

  CCP1CON = b(00001100);
  PR2 = 0xFF;
  T2CON = b(00000100);
  CCPR1L = 0x00;

  debounce_init(hours_debounce,hours_switch_raw);
  debounce_init(mins_debounce,mins_switch_raw);
  debounce_init(secs_debounce,secs_switch_raw);

  if (!hours_switch_raw && !mins_switch_raw && secs_switch_raw){
    mode = fast_clock;
  } else if (hours_switch_raw && !mins_switch_raw && !secs_switch_raw){
    mode = slow_clock;
  } else if (!hours_switch_raw && mins_switch_raw && !secs_switch_raw){
    // Oooh! Metrics display mode! Don't see that very often...
    inc_metric_meta();

    // Basically just cycle through the bytes in the eeprom and display them on
    // screen forever.

    i = 0;
    while (1){
      display_digits(
                     i / 16,i % 16,
                     CHAR_BLANK,CHAR_BLANK,
                     (((uint8_t *)(&eeprom_data))[i] / 16),
                      ((uint8_t *)(&eeprom_data))[i] % 16);

      delay10tcy(1);

      if (debounce_just_pressed(hours_debounce,hours_switch_raw)){
        i++;
        if (i >= sizeof(eeprom_data))
          i = 0;
      }
      if (debounce_just_pressed(secs_debounce,secs_switch_raw)){
        i--;
        if (i >= sizeof(eeprom_data))
          i = sizeof(eeprom_data) - 1;
      }

      debounce_add_sample(hours_debounce,hours_switch_raw);
      debounce_add_sample(secs_debounce,secs_switch_raw);
    }

  } else {
    mode = none_chosen;
  }

  if (mode == none_chosen){
    mode = eeprom_read_uint32(EEPROM_ADDR_MODE);  
  } else {
    eeprom_write_uint32(mode,EEPROM_ADDR_MODE);
    trigger_save_eeprom();
  }

  silly_hour_display = eeprom_read_uint32(EEPROM_ADDR_SILLY_HOUR_DISPLAY);
}

// Display time to the display in a nice pretty way.
void display_time(){
  uint8_t adj_secs,adj_mins,adj_hours,h0,h1;

  // Adjust seconds based on fast, or slow, mode.
  if (mode == fast_clock){
    // Pretty easy, counting from (0-140)/2, display 0-60 twice.
    adj_secs = (time_secs / (secs_to_real_secs_divider / 2)) % 60;
  } else {
    // Slow clock.
    //
    // Harder, need to look at what minute it is as well.
    adj_secs = (time_secs / (secs_to_real_secs_divider * 2)) + ((time_mins % 2) * 30);
  }


  set_dots(false,true,true,true,true,false);

  //  By popular request... blah.
  adj_hours = time_hours;
  if (silly_hour_display){
    if (adj_hours >= 12){
      adj_hours = adj_hours - 12;
    } else {
      adj_hours = adj_hours;
      set_dots(false,false,true,true,true,false);
    }
    if (!adj_hours)
      adj_hours = 12; // blah, so much work for such sillyness
  }
  adj_hours = to_bcd(adj_hours);

  h0 = adj_hours / 16;
  if (!h0 && silly_hour_display)
    h0 = CHAR_BLANK; // remove leading zeros for hours
                     // admittedly this might be a nice feature for
                     // 24 hour mode too, although most standards include the
                     // leading zero.
                     
  h1 = adj_hours % 16;


  adj_mins = to_bcd(time_mins);
  adj_secs = to_bcd(adj_secs);
  display_digits(
                 h0,h1,
                 adj_mins / 16, adj_mins % 16,
                 adj_secs / 16, adj_secs % 16);
}

void check_for_keypresses(){

  if (debounce_just_pressed(hours_debounce,hours_switch_raw)){
    inc_hours();
    inc_metric_sw_hours();
  } else if (debounce_held_down(hours_debounce,hours_switch_raw)) {
    silly_hour_display = !silly_hour_display;
    eeprom_write_uint32(silly_hour_display,EEPROM_ADDR_SILLY_HOUR_DISPLAY);
    trigger_save_eeprom();

    // Be user friendly and undo the initial increment.
    dec_hours();
  }

  if (debounce_just_pressed(mins_debounce,mins_switch_raw)){
    inc_mins();
    inc_metric_sw_mins();
  }

  if (debounce_just_pressed(secs_debounce,secs_switch_raw)){
    clear_secs();
    inc_metric_sw_secs();
  }

  debounce_add_sample(hours_debounce,hours_switch_raw);
  debounce_add_sample(mins_debounce,mins_switch_raw);
  debounce_add_sample(secs_debounce,secs_switch_raw);
}

void check_brightness(){
  // Check the brightness knob and update the pwm
  ADCON0bits.GO = 1;
  while (ADCON0bits.GO);
  CCPR1L = ADRESH;
}

void do_user(){
  check_brightness();
  check_for_keypresses();
  display_time();
}

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:
