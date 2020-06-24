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
        self.current = array(current)


def DiodeCurrent(v, A, B, C):
    return exp(A*v**2 + B*v + C) - exp(C)


def Plot(data, A, B, C, outImageFileName = None):
    plt.figure(figsize=(11, 8.5), dpi=100)
    model = DiodeCurrent(data.voltage, A, B, C)
    plt.plot(data.voltage, model, 'b-')
    plt.plot(data.voltage, data.current, 'r.', markersize=3)
    #plt.title(f.title or filename)
    plt.xlabel('Voltage [V]')
    plt.ylabel('Current [mA]')
    plt.grid()
    if outImageFileName:
        plt.savefig(outImageFileName, dpi=100, bbox_inches='tight')
    else:
        plt.show()
    plt.close('all')    # free all memory


def FitCurve(filename):
    data = DataFile(filename)
    [A, B, C], _ = curve_fit(DiodeCurrent, data.voltage, data.current, maxfev=50000)
    print('A={}, B={}, C={}'.format(A, B, C))
    Plot(data, A, B, C)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        sys.exit(FitCurve(sys.argv[1]))
    print('''
USAGE:

model.py infile.csv
''')
    sys.exit(1)
