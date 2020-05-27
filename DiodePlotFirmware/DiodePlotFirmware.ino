/*
    DiodePlotFirmware.ino  -  Don Cross  -  2020-05-27

    Arduino Uno firmware for measuring current/voltage
    relationship for a diode.
*/

#define BINARY(a,b,c,d,e,f,g,h) ((a)<<7|(b)<<6|(c)<<5|(d)<<4|(e)<<3|(f)<<2|(g)<<1|(h))


void setup()
{
    PIND   = BINARY(0,0,0,0,0,0,0,0);       // disable all pullup resistors on D register
    PORTD  = BINARY(0,0,0,0,0,0,0,0);       // turn off all output pins on D register
    DDRD   = BINARY(1,1,1,1,1,1,1,1);       // all D register pins are outputs

    Serial.begin(115200);
}

void loop()
{
    ++PORTD;
    delay(50);
    int v1 = analogRead(0);     // op-amp output voltage
    int v2 = analogRead(1);     // voltage on diode's anode

    Serial.print((int)PORTD);
    Serial.print(" ");
    Serial.print(v1);
    Serial.print(" ");
    Serial.print(v2);
    Serial.println();
}