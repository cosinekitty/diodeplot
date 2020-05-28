/*
    DiodePlotFirmware.ino  -  Don Cross  -  2020-05-27

    https://github.com/cosinekitty/diodeplot

    Arduino Uno firmware for measuring current/voltage
    relationship for a diode.
*/

enum RunState
{
    WAITING,
    RUNNING,
};

RunState state = WAITING;
int trial = 0;
int limit = 10;
int accum;

void setup()
{
    PIND   = 0x00;       // disable all pullup resistors on D register
    PORTD  = 0x00;       // turn off all output pins on D register
    DDRD   = 0xff;       // all D register pins are outputs

    Serial.begin(115200);
    delay(1000);
    Serial.println("# READY");
}


void Sample()
{
    delay(1);      // Wait a little while for voltage to stabilize.

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


void loop()
{
    switch (state)
    {
    case WAITING:
        break;      // do nothing

    case RUNNING:
        if (PORTD == 0)
        {
            // Should we start another trial run, or are we finished?
            ++trial;
            if (trial > limit)
            {
                // We have finished.
                Serial.println("# FINISHED");
                state = WAITING;
                return;
            }

            Serial.print("# trial ");
            Serial.print(trial);
            Serial.print(" of ");
            Serial.print(limit);
            Serial.println();
        }

        Sample();

        // We cycle through all possible combination of 8 binary outputs on the D register:
        // 0 through 255, to produce 256 possible voltage levels.
        ++PORTD;
        break;
    }

    while (Serial.available() > 0)
    {
        char c = Serial.read();
        switch (c)
        {
        case 'z':       // reset and print accumulator
            accum = 0;
            // fall through to print
        case 'a':       // print accumulator
            Serial.print("# accum = ");
            Serial.print(accum);
            Serial.println();
            break;

        case 'r':       // run or restart
            state = RUNNING;
            trial = 0;
            limit = accum;
            accum = 0;
            break;

        case 'h':       // halt
            state = WAITING;
            PORTD = 0;          // turn off the output voltage
            Serial.println("# HALTED");
            break;

        case 'v':       // set voltage to accumulator and halt
            state = WAITING;
            PORTD = accum;
            accum = 0;
            Sample();
            break;

        default:
            if (c >= '0' && c <= '9')
                accum = (10 * accum) + (c - '0');
                break;
        }
    }
}
