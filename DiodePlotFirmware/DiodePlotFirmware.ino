/*
    DiodePlotFirmware.ino  -  Don Cross  -  2020-05-27

    Arduino Uno firmware for measuring current/voltage
    relationship for a diode.
*/

#define BINARY(a,b,c,d,e,f,g,h) ((a)<<7|(b)<<6|(c)<<5|(d)<<4|(e)<<3|(f)<<2|(g)<<1|(h))


void setup()
{
    DDRD   = BINARY(1,1,1,1,1,1,1,1);       // all D register pins are outputs
    PORTD  = BINARY(0,0,0,0,0,0,0,0);       // disable pullup resistors on all outputs
}

void loop()
{

}