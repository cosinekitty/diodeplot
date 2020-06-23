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


def DiodeCurrent(voltage, v0, alpha):
    return exp(alpha*(voltage - v0))


def Plot(data, v0, alpha, outImageFileName = None):
    plt.figure(figsize=(11, 8.5), dpi=100)
    model = DiodeCurrent(data.voltage, v0, alpha)
    plt.plot(data.voltage, model, 'g-')
    plt.plot(data.voltage, data.current, 'b.', markersize=3)
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
    [v0, alpha], _ = curve_fit(DiodeCurrent, data.voltage, data.current)
    print(v0, alpha)
    Plot(data, v0, alpha)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        sys.exit(FitCurve(sys.argv[1]))
    print('''
USAGE:

model.py infile.csv
''')
    sys.exit(1)
