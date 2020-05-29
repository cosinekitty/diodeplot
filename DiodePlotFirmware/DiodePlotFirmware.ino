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
int level;      // the value 0..255 for the output voltage we are trying to produce


void SetLevel(int n)
{
    level = n & 0xff;
    PORTB = (PORTB & ~3) | (level & 3);     // set low 2 bits on B register
    PORTD = (PORTD & 3)  | (level & ~3);    // set high 6 bits on D register
}


void setup()
{
    PIND   &=  3;             // disable top 6 pullup resistors on D register
    DDRD   |= ~3;             // top 6 output pins on D register are outputs

    PINB   &=  ~3;            // disable bottom 2 pullup resistors on B register
    DDRB   |=   3;            // bottom 2 output pins on B register are outputs

    SetLevel(0);              // start with the lowest output voltage

    delay(500);
    Serial.begin(115200);     // overrides settings for PD0 (RX), PD1 (TX)
    delay(500);
    Serial.println("# READY");
}



void Sample()
{
    delay(1);      // Wait a little while for voltage to stabilize.

    int v1 = analogRead(0);     // op-amp output voltage
    int v2 = analogRead(1);     // voltage on diode's anode

    // Log the data.
    Serial.print(level);
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
        if (level == 0)
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

        // We cycle through all possible combination of 8 binary outputs:
        // 0 through 255, to produce 256 possible voltage levels.
        SetLevel(level + 1);
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
            SetLevel(0);   // turn off the output voltage
            Serial.println("# HALTED");
            break;

        case 'v':       // set voltage to accumulator and halt
            state = WAITING;
            SetLevel(accum);
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
