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

#ifndef TIME_H
#define TIME_H

#define num_time_tics  (16)
#define num_time_secs  (240)
#define num_time_mins  (60)
#define num_time_hours (24)

#define secs_to_real_secs_divider (num_time_secs / 60)

extern volatile uint8_t time_tics,time_secs,time_mins,time_hours;

void init_time();

void do_time();

// As their name says... For the user interface code.
// *_mins() does not carry on overflow. 13:59 + 1 -> 13:00
void inc_hours();
void dec_hours();
void inc_mins();
//void dec_mins();
void clear_secs();

// Local Variables:
// mode: C
// fill-column: 76
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
// vim: et:sw=2:sts=2:ts=2:cino=>2s,{s,\:s,+s,t0,g0,^-2,e-2,n-2,p2s,(0,=s:

#endif
