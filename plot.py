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
    # Based on linear regression analysis, comparing
    # analog input values (0..1023) versus voltage readings
    # taken with a RadioShack multimeter.
    return (0.004983 * a) + 0.010523

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
            return (Voltage(self.a1)-Voltage(self.a2)) / R9
        raise Exception('Invalid variable name: ' + varname)

    def __repr__(self):
        return '[n={}, a1={}, a2={}]'.format(self.n, self.a1, self.a2)


def Mean(center, hist):
    n = len(hist)
    if (n < 1) or (n & 1) != 1:
        raise Exception('Invalid histogram length: {}'.format(n))
    middle = (n-1) // 2
    samples = sum(hist)
    if samples <= 0:
        raise Exception('Total count was not positive')
    weight = sum(hist[i] * (center+(i-middle)) for i in range(n))
    return weight / samples


class DataFile:
    def __init__(self, filename):
        lnum = 0
        self.data = []
        self.filename = filename
        self.title = ''
        direction = +1  # assume FORWARD bias unless we see otherwise
        with open(filename, 'rt') as infile:
            for line in infile:
                line = line.strip()
                lnum += 1
                if line == '':
                    continue
                if line.startswith('#'):
                    line = line[1:].strip()
                    if line.startswith('[') and line.endswith(']'):
                        self.title = line[1:-1]
                    continue
                if line == 'FORWARD':
                    direction = +1
                    continue
                if line == 'REVERSE':
                    direction = -1
                    continue
                # Look for "multisampling histogram" format that looks like this;
                # 26 104 [0 0 33 961 6 0 0] 77 [0 0 13 782 205 0 0]
                m = re.match(r'^(\d+)\s+(\d+)\s+\[([^\]]*)\]\s+(\d+)\s+\[([^\]]*)\]\s*$', line)
                if m:
                    n = int(m.group(1))
                    a1 = int(m.group(2))
                    h1 = [int(s) for s in m.group(3).split()]
                    a2 = int(m.group(4))
                    h2 = [int(s) for s in m.group(5).split()]
                    self.data.append(Sample(direction * n, direction * Mean(a1,h1), direction * Mean(a2,h2)))
                    continue
                raise Exception('Invalid data format at {} line {}'.format(filename, lnum))


def Description(varname):
    if varname == 'n':
        return 'digital output'
    if varname == 'v1':
        return 'op-amp output voltage'
    if varname == 'v2':
        return 'device voltage'
    if varname == 'i':
        return 'device current [mA]'
    raise Exception('Unknown variable: ' + varname)


def PlotFile(xvar, yvar, filename):
    f = DataFile(filename)
    xlist = []
    ylist = []
    for d in f.data:
        xlist.append(d.GetVar(xvar))
        ylist.append(d.GetVar(yvar))
    plt.plot(xlist, ylist, 'b.')
    plt.title(f.title or filename)
    plt.xlabel(Description(xvar))
    plt.ylabel(Description(yvar))
    plt.grid()
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
