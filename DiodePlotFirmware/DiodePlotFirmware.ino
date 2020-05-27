/*
    DiodePlotFirmware.ino  -  Don Cross  -  2020-05-27

    https://github.com/cosinekitty/diodeplot

    Arduino Uno firmware for measuring current/voltage
    relationship for a diode.
*/

void setup()
{
    PIND   = 0x00;       // disable all pullup resistors on D register
    PORTD  = 0x00;       // turn off all output pins on D register
    DDRD   = 0xff;       // all D register pins are outputs

    Serial.begin(115200);

    delay(5000);        // wait 5 seconds before starting the test
}

void loop()
{
    // We cycle through all possible combination of 8 binary outputs:
    // 0 through 255, to produce 256 possible voltage levels.
    ++PORTD;

    delay(50);      // Wait a little while for voltage to stabilize.

    int v1 = analogRead(0);     // op-amp output voltage
    int v2 = analogRead(1);     // voltage on diode's anode

    // Log the data.
    Serial.print((int)PORTD);
    Serial.print(" ");
    Serial.print(v1);
    Serial.print(" ");
    Serial.print(v2);
    Serial.println();
}
