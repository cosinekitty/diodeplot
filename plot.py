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

class Sample:
    def __init__(self, n, a1, a2):
        self.n = n      # The DPORT value: 0..255
        self.a1 = a1    # The analog reading for V1 = voltage output from op-amp
        self.a2 = a2    # The analog reading for V2 = voltage between resistor and test device

    def GetVar(self, varname):
        if varname == 'n':
            return self.n
        if varname == 'v1':
            return Voltage(self.a1)
        if varname == 'v2':
            return Voltage(self.a2)
        if varname == 'i':
            return (Voltage(self.a1-self.a2)) / R9
        raise Exception('Invalid variable name: ' + varname)


def LoadData(filename):
    lnum = 0
    data = []
    with open(filename, 'rt') as infile:
        for line in infile:
            line = line.strip()
            lnum += 1
            if (line != '') and (not line.startswith('#')):
                m = re.match(r'^(\d+)\s+(\d+)\s+(\d+)$', line)
                if not m:
                    print('ERROR({} line {}): invalid data format'.format(filename, lnum))
                    sys.exit(1)
                n = int(m.group(1))
                a1 = int(m.group(2))
                a2 = int(m.group(3))
                data.append(Sample(n, a1, a2))
    return data


def PlotFile(xvar, yvar, filename):
    data = LoadData(filename)
    xlist = []
    ylist = []
    for d in data:
        xlist.append(d.GetVar(xvar))
        ylist.append(d.GetVar(yvar))
    plt.plot(xlist, ylist, 'b.')
    plt.show()
    plt.close('all')    # free all memory
    return 0


if __name__ == '__main__':
    if len(sys.argv) == 4:
        sys.exit(PlotFile(sys.argv[1], sys.argv[2], sys.argv[3]))

    print(r'''
USAGE:  plot.py x y datafile.txt

Where x and y are each one of the following:

    n  = the DPORT value 0..255 that is fed into the resistor network.
    v1 = measured voltage coming out of the op-amp.
    v2 = measured voltage fed into the test device.
    i  = deduced current calculated as (v1-v2)/R.
''')
    sys.exit(1)
