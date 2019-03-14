// UMU:
//   (1) (6)
// (2) (7) (5)
//   (3) (4)
// 1 - DPA (clk)
// 3 - DPB (dir, 1 - forward, 0 - backward)
// 6 - GND
// Arduino:
// 2 - DPA
// 4 - DPB

// stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
// tail -f /dev/ttyACM0
// echo "move 1" >> /dev/ttyACM0

// #define DEBUG

#define __STRING(s) #s
#define __VALUE(x) __STRING(x)

#ifdef DEBUG
#    define TRACE(x) do { Serial.print("(" __VALUE(__LINE__) ")" #x ":'"); Serial.print(x); Serial.println("'"); } while(0)
#    define MESSAGE(msg) do { Serial.println("(" __VALUE(__LINE__) ")" msg ); } while(0)
#else
#    define TRACE(x) do {} while(0)
#    define MESSAGE(msg) do {} while(0)
#endif

void usage()
{
    Serial.println("Commands:");
    Serial.println("  help");
    Serial.println("  move params");
    Serial.println("    where params are:");
    Serial.println("    speed=km/h");
    Serial.println("    steps=count");
    Serial.println("    dir=forw|back");
    Serial.println("    duration=milliseconds|inf");
    Serial.println("      duration affects steps");
    Serial.println("  correction mm*100 - path encoder correction in millimeters * 100");
    Serial.println("  pause milliseconds");
    Serial.println("Commands can be separated by semicolons.");
}

#if defined(__MSP430F5529__)
#    define DPA P2_2
#    define DPB P2_4
#else
#    define DPA 2
#    define DPB 4
#endif

void setup()
{
    Serial.begin(9600);
    while (!Serial) {}

    usage();

    pinMode(DPA, OUTPUT);
    digitalWrite(DPA, LOW);
    pinMode(DPB, OUTPUT);
    digitalWrite(DPB, LOW);
}

static const int maxParams = 4;

typedef enum t_Direction {
    unknown = 0,
    forward = 1,
    backward = -1
} Direction;

Direction _dir = unknown;

void setDirection(Direction dir);
void setDirection(Direction dir)
{
    if (_dir == dir) {
        return;
    }
    _dir = dir;

    if (dir == forward) {
        digitalWrite(DPB, LOW);
    }
    else {
        digitalWrite(DPB, HIGH);
    }
}

void setDirection(const String & string)
{
    Direction dir = unknown;
    if (string.equals("forw")) {
        MESSAGE("dir:forward");
        dir = forward;
    }
    else if (string.equals("back")) {
        MESSAGE("dir:backward");
        dir = backward;
    }

    setDirection(dir);
}

void oneStep(unsigned long delayUsec)
{
    delayUsec /= 6;
    MESSAGE("step");
    for (int i = 0; i < 3; ++i) {
        digitalWrite(DPA, HIGH);
        delayMicroseconds(delayUsec);
        digitalWrite(DPA, LOW);
        delayMicroseconds(delayUsec);
    }
}

int pathEncoderCorrection = 100;
void executeCmd(const String & cmd, const String (& params)[maxParams])
{
    Serial.print(cmd);
    Serial.print(" ");
    for (int i = 0; i < maxParams; ++i) {
        if (params[i].length() == 0) {
            break;
        }
        Serial.print(params[i]);
        Serial.print(" ");
    }
    Serial.println("");

    if (cmd.equals("help")) {
        usage();
    }
    else if (cmd.equals("move")) {
        unsigned long steps = 0;
        unsigned long duration = 0;
        int isInf = 0;
        unsigned long delayUs = 60000;

        for (int i = 0; i < maxParams; ++i) {
            if (params[i].length() == 0) {
                break;
            }
            int index = params[i].indexOf("=");
            if (index < 0 || index == (params[i].length() - 1)) {
                Serial.print("Wrong param '");
                Serial.print(params[i]);
                Serial.println("'");
                usage();
                return;
            }
            String param = params[i].substring(0, index);
            param.trim();
            String arg = params[i].substring(index + 1);
            arg.trim();
            if (param.equals("speed")) {
                float speed = arg.toFloat();
                delayUs = (3600. * pathEncoderCorrection) / (100 * speed);
                delayUs -= 60; // compensation
                Serial.print("delay = ");
                Serial.print(delayUs / 3);
                Serial.println(" us");
                TRACE(delayUs);
            }
            else if (param.equals("steps")) {
                steps = arg.toInt();
            }
            else if (param.equals("dir")) {
                setDirection(arg);
            }
            else if (param.equals("duration")) {
                if (arg.equals("inf")) {
                    isInf = 1;
                }
                else {
                    isInf = 0;
                    duration = arg.toInt();
                }
            }
            else {
                Serial.print("Unknown param '");
                Serial.print(param);
                Serial.println("'");
                usage();
                return;
            }
        }

        if (isInf) {
            for (;;) {
                oneStep(delayUs);
            }
        }
        else if (duration) {
            unsigned long timeBeg = millis();
            while((millis() - timeBeg) <= duration) {
                oneStep(delayUs);
            }
        }
        else {
            for (int i = 0; i < steps; ++i) {
                oneStep(delayUs);
            }
        }
        Serial.println("Ok");
    }
    else if (cmd.equals("correction")) {
        if (params[0].length() != 0) {
            pathEncoderCorrection = params[0].toInt();
        }
        Serial.println("Ok");
    }
    else if (cmd.equals("pause")) {
        int delayUsec = params[0].toInt();
        delay(delayUsec);
        Serial.println("Ok");
    }
    else {
        usage();
    }
}

void parseCmd(const String & string)
{
    String cmd;
    String params;
    int i;
    String paramsArray[maxParams];

    TRACE(string);

    int index = 0;
    index = string.indexOf(" ");
    if (index > 0) {
        cmd = string.substring(0, index);
        params = string.substring(index + 1);
    }
    else {
        cmd = string;
        params = String();
    }
    params.trim();
    TRACE(cmd);
    TRACE(params);

    for (i = 0; i < maxParams; ++i) {
        paramsArray[i] = String();
    }

    if (params.length() !=0 ) {
        i = 0;
        do {
            index = params.indexOf(" ");
            if (index == 0) {
                params = params.substring(1);
                continue;
            }
            else if (index > 0) {
                paramsArray[i] = params.substring(0, index);
                params = params.substring(index + 1);
            }
            else {
                paramsArray[i] = params;
                params = String();
            }
            ++i;
        }
        while (params.length() != 0 && i < maxParams);
    }

    executeCmd(cmd, paramsArray);
}

void loop()
{
    String incomingString;
    char incomingByte;
    String cmd;

    incomingString = String();
    do {
        for(;;) {
            while (!Serial.available()) {};
            incomingByte = Serial.read();
            if (incomingByte == '\n' || incomingByte == '\r') {
                break;
            }
            incomingString += incomingByte;
        }
    }
    while (incomingString.length() == 0);
    incomingString.toLowerCase();
    TRACE(incomingString);

    int index = 0;
    do {
        index = incomingString.indexOf(";");
        if (index == 0) {
            cmd = String();
            incomingString = incomingString.substring(1);
            continue;
        }
        else if (index > 0) {
            cmd = incomingString.substring(0, index);
            incomingString = incomingString.substring(index + 1);
        }
        else {
            cmd = incomingString;
            incomingString = String();
        }

        cmd.trim();
        parseCmd(cmd);
    }
    while (incomingString.length() != 0);
}

