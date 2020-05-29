/*
    DiodePlotFirmware.ino  -  Don Cross  -  2020-05-27

    https://github.com/cosinekitty/diodeplot

    Arduino Uno firmware for measuring current/voltage
    relationship for a diode.
*/

enum RunState
{
    WAITING,            // sitting idle, waiting for a command from the user
    SAMPLING,           // sweep through voltages, sampling each once, for the number of trials in 'accum'
    MULTI_SAMPLING,     // sweep through voltages once, but sample each 'accum' times
};

RunState state = WAITING;
int trial = 0;
int limit = 10;
int accum;
int level;      // the value 0..255 for the output voltage we are trying to produce


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


void loop()
{
    switch (state)
    {
    case WAITING:
        break;      // do nothing

    case SAMPLING:
        SingleSample();
        break;

    case MULTI_SAMPLING:
        MultiSample();
        break;
    }

    ProcessSerialInput();
}


void SetLevel(int n)
{
    level = n & 0xff;
    PORTB = (PORTB & ~3) | (level & 3);     // set low 2 bits on B register
    PORTD = (PORTD & 3)  | (level & ~3);    // set high 6 bits on D register
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


void SingleSample()
{
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
}


void Halt()
{
    state = WAITING;
    SetLevel(0);   // turn off the output voltage
    Serial.println("# HALTED");
}


const int MAX_READING_SPREAD = 8;       // how far from the initial analog reading do we tolerate?
const int HISTOGRAM_BUFFER = (2 * MAX_READING_SPREAD) + 1;

class Histogram
{
private:
    int center;     // the first analog reading defines the "center" of the array at index tally[MAX_READING_SPREAD].
    unsigned tally[HISTOGRAM_BUFFER];

public:
    Histogram()
        : center(-1)        // indicates we have not ready any data yet
    {
        for (int i=0; i < HISTOGRAM_BUFFER; ++i)
            tally[i] = 0;
    }

    bool Add(int analog)
    {
        if (center < 0)
            center = analog;

        int index = (analog-center) + MAX_READING_SPREAD;
        if (index < 0 || index >= HISTOGRAM_BUFFER)
        {
            // The reading went outside our window.
            // If possible, adjust the window so we can continue the experiment.
            if (index == -1 && tally[HISTOGRAM_BUFFER-1] == 0)
            {
                // We can move everything right by one slot.
                --center;
                for (int i=HISTOGRAM_BUFFER-1; i > 0; --i)
                    tally[i] = tally[i-1];
                tally[0] = 1;   // include the new data point
                return true;
            }

            if (index == HISTOGRAM_BUFFER && tally[0] == 0)
            {
                // We can move everything left by one slot.
                ++center;
                for (int i=0; i < HISTOGRAM_BUFFER-1; ++i)
                    tally[i] = tally[i+1];
                tally[HISTOGRAM_BUFFER-1] = 1;  // include the new data point
                return true;
            }

            // We just can't salvage this data.
            return false;
        }

        ++tally[index];
        return true;
    }

    void Print() const
    {
        // Print the smallest possible central section of the tally array
        // without omitting any nonzero counts.
        int left = 0;
        int right = HISTOGRAM_BUFFER - 1;
        while (tally[left] == 0 && tally[right] == 0 && left < right)
        {
            ++left;
            --right;
        }

        Serial.print(" ");
        Serial.print(center);
        Serial.print(" [");
        for (int index = left; index <= right; ++index)
        {
            if (index > left)
                Serial.print(" ");
            Serial.print(tally[index]);
        }
        Serial.print("]");
    }
};


void MultiSample()
{
    Histogram h1;   // histogram for v1 readings
    Histogram h2;   // histogram for v2 readings

    delay(1);

    // Take a series of measurements and tally the different measurements.
    for (int count=0; count < limit; ++count)
    {
        int v1 = analogRead(0);     // op-amp output voltage
        int v2 = analogRead(1);     // voltage on diode's anode

        if (!h1.Add(v1) || !h2.Add(v2))
        {
            // Reading was too erratic to fit inside the histogram.
            Serial.println("# FAIL: readings were too erratic");
            Halt();
            return;
        }
    }

    // Write the results to the serial monitor.
    Serial.print(level);
    h1.Print();
    h2.Print();
    Serial.println();

    // We cycle through all possible combination of 8 binary outputs:
    // 0 through 255, to produce 256 possible voltage levels.
    SetLevel(level + 1);
    if (level == 0)
    {
        // We have finished the multi-sample trial.
        Serial.println("# FINISHED");
        state = WAITING;
        return;
    }
}


void ProcessSerialInput()
{
    while (Serial.available() > 0)
    {
        char c = Serial.read();
        switch (c)
        {
        case 'a':       // print accumulator; can also use this to test the serial connection
            Serial.print("# accum = ");
            Serial.print(accum);
            Serial.println();
            break;

        case 'h':       // halt
            Halt();
            break;

        case 'm':       // multi-sample mode
            state = MULTI_SAMPLING;
            trial = 0;
            limit = accum;
            accum = 0;
            SetLevel(0);
            break;

        case 'r':       // run or restart
            state = SAMPLING;
            trial = 0;
            limit = accum;
            accum = 0;
            SetLevel(0);
            break;

        case 'v':       // set voltage to accumulator and halt
            state = WAITING;
            SetLevel(accum);
            accum = 0;
            Sample();
            break;

        default:
            // Append a decimal digit to the accumulator.
            // Does not check for integer overflow.
            if (c >= '0' && c <= '9')
                accum = (10 * accum) + (c - '0');
            break;
        }
    }
}
