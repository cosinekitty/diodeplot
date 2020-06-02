# Current/Voltage Plotter

This is a microcontroller project to measure and plot the current/voltage curve for a two-terminal device.
It uses Arduino-based hardware to apply different voltages to a device and measure the current that flows through it. 
The data is captured over the serial monitor and fed through a Python script that plots the graph. 
Here is an example using a 3V Zener diode:

![Zener diode plot](https://raw.githubusercontent.com/cosinekitty/diodeplot/master/data/zener3v.png)

# Curcuit Schematic

![Circuit schematic](https://raw.githubusercontent.com/cosinekitty/diodeplot/master/schematics/dac.png)

# Read More

See the article I wrote explaining this project:
[Build A Homebrew Current/Voltage Plotter](https://towardsdatascience.com/build-a-homebrew-current-voltage-plotter-88413dfa44d0)
