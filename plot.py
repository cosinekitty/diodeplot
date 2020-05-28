#!/usr/bin/env python3
#
#   plot.py  -  Don Cross  -  2020-05-27
#
#   https://github.com/cosinekitty/diodeplot
#
#   Program that plots current/voltage data captured by
#   the Arduino program DiodePlotFirmware.ino.
#

import re
import sys
import matplotlib.pyplot as plt

R9 = 0.3272     # measured value of the resistor in series with the diode, in K ohms

def Voltage(a):
    return 0.005 * a

def PlotFile(filename):
    lnum = 0
    vlist = []      # list of anode voltages in V
    ilist = []      # list of currents in mA
    with open(filename, 'rt') as infile:
        for line in infile:
            line = line.strip()
            lnum += 1
            if (line != '') and (not line.startswith('#')):
                m = re.match(r'^(\d+)\s+(\d+)\s+(\d+)$', line)
                if not m:
                    print('ERROR({} line {}): invalid data format'.format(filename, lnum))
                    return 1
                #n = int(m.group(1))
                a1 = int(m.group(2))
                a2 = int(m.group(3))
                v1 = Voltage(a1)
                v2 = Voltage(a2)
                i = (v1 - v2) / R9
                vlist.append(v2)
                ilist.append(i)
    plt.plot(vlist, ilist, 'b.')
    plt.show()
    plt.close('all')    # free all memory
    return 0

if __name__ == '__main__':
    if len(sys.argv) == 2:
        sys.exit(PlotFile(sys.argv[1]))

    print('USAGE:  plot.py datafile.txt')
    sys.exit(1)
