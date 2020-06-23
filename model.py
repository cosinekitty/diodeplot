#!/usr/bin/env python3
#
#   model.py  -  Don Cross
#
#   https://github.com/cosinekitty/diodeplot
#
#   Demonstrates using scipy to fit a curve to experimental data.
#
import sys
import csv
import matplotlib.pyplot as plt
from numpy import array, exp
from scipy.optimize import curve_fit


class DataFile:
    def __init__(self, filename):
        voltage = []
        current = []
        with open(filename, 'rt') as infile:
            reader = csv.reader(infile)
            lnum = 0
            for row in reader:
                lnum += 1
                if lnum == 1:
                    if row != ['device voltage', 'device current [mA]']:
                        print('ERROR: invalid columns:', row)
                        raise Exception('Invalid data file format')
                else:
                    voltage.append(float(row[0]))
                    current.append(float(row[1]))
        self.voltage = array(voltage)
        self.current = array(voltage)

def DiodeCurrent(voltage, v0, alpha):
    return exp(alpha*(voltage - v0))


def FitCurve(filename):
    data = DataFile(filename)
    print(curve_fit(DiodeCurrent, data.voltage, data.current))

if __name__ == '__main__':
    if len(sys.argv) == 2:
        sys.exit(FitCurve(sys.argv[1]))
    print('''
USAGE:

model.py infile.csv
''')
    sys.exit(1)
