#!/usr/bin/python
# vim: tabstop=4 expandtab shiftwidth=4 fileencoding=utf8
# (c) 2007 Peter Todd <pete@petertodd.org>
#
# This program is made available under the GNU GPL version 3.0 or
# greater. See the accompanying file COPYING for details.
#
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.


def get_long(base_addr,name):
    b = []
    for i in range(base_addr,base_addr + 4):
        b.append(raw_input('%s %x: ' % (name,i)))

    # convert to int from hex
    b = [int(d,16) for d in b]

    # create a long from the little endian
    return b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24)


metrics_names = ('total_running_time','hours_sw','minutes_sw','seconds_sw','max_temperature','min_temperature','metrics_mode_usage','fast_or_slow','silly_hours')

i = 0

metrics_vals = {}
for m in metrics_names:
    metrics_vals[m] = get_long(i,m)
    i += 4

for m in metrics_names:
    print '%s - %d' % (m,metrics_vals[m])
