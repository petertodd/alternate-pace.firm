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
#include <time.h>
#include <ds3231.h>
#include <metrics.h>

volatile uint8_t time_tics,time_secs,time_mins,time_hours;

void init_time(){
  init_ds3231();

//  time_tics = time_secs = time_mins = time_hours = 0;

  delay100ktcy(1);

  read_time_from_ds3231();
}

void inc_hours(){
  time_hours++;
  if (time_hours >= num_time_hours)
    time_hours = 0;
  write_time_to_ds3231(ds3231_write_hours);
}

void dec_hours(){
  time_hours--;
  if (time_hours >= num_time_hours)
    time_hours = num_time_hours - 1;
  write_time_to_ds3231(ds3231_write_hours);
}

void inc_mins(){
  time_mins++;
  if (time_mins >= num_time_mins)
    time_mins = 0;
  write_time_to_ds3231(ds3231_write_mins);
}

#if 0 // not used
void dec_mins(){
  time_mins--;
  if (time_mins >= num_time_mins)
    time_mins = num_time_mins - 1;
  write_time_to_ds3231(ds3231_write_mins);
}
#endif

void clear_secs(){
  time_secs = 0;
  time_tics = 0;
  write_time_to_ds3231(ds3231_write_secs);
}

void do_time(){
  uint8_t old_time_hours,old_time_mins,old_time_secs;
  time_tics++;
  // == so time_tics-- works as expected
  if (time_tics == num_time_tics){
    time_tics = 0;

    // "seconds" aren't what they seem...
    if (!(time_secs % secs_to_real_secs_divider))
      inc_metric_running_time();

    time_secs++;
    if (time_secs >= num_time_secs){
      time_secs = 0;

      // Try to resync a bit... The current timer1 code, even though it's being
      // fed from the clean 32khz output, seems to be a little slow.
      
      old_time_hours = time_hours;
      old_time_mins = time_mins;
      old_time_secs = time_secs;
      read_time_from_ds3231();

      // If we're on track, seconds will == 0 too. Otherwise push or pull
      // time_tics a bit to adjust the time. 
      if (time_secs){
        if (time_secs > num_time_secs / 2){
          // Running fast.
          time_tics--;
        } else {
          // Running slow.
          time_tics++;
        }
      }

      time_hours = old_time_hours;
      time_mins = old_time_mins;
      time_secs = old_time_secs;

      time_mins++;
      if (time_mins >= num_time_mins){
        time_mins = 0;

        // Hourly is a good time to save the metrics from the imperial forces.
        //
        // Sadly decimal time never quite made it...
        save_metrics();

        time_hours++;
        if (time_hours >= num_time_hours){
          time_hours = 0;
        }
      }
    }
  }
}

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:
