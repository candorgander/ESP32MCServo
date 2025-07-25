#ifndef ESP32_MCSERVO_H
#define ESP32_MCSERVO_H

#include "ESP32MCPWM.h"

// Arduino Servo Defaults
#define SERVO_MIN_PULSEWIDTH_US (400)  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US (2400) // Maximum pulse width in microsecond

#ifdef ARDUINO_SERVO

class Servo
{

public:
    Servo();

    // Arduino Servo Library calls
    int attach(int pin);                   // attach the given pin to the next free channel, returns channel number or -1 if failure
    int attach(int pin, int min, int max); // as above but also sets min and max values for writes.
    void detach();
    void write(int value);             // Write pulse width in angles. If value > SERVO_MIN_PULSEWIDTH_US, the it is taken as mircoseconds (For Arduino compatibility. Why?)
    void writeMicroseconds(int value); // Write pulse width in microseconds
    void release();                    // Opposite of write, the servo stops holding the angle passed by write()
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds();            // returns current pulse width in microseconds for this servo
    bool attached();                   // return true if this servo is attached, otherwise false

private:
    int min = SERVO_MIN_PULSEWIDTH_US; // minimum pulse width for this servo
    int max = SERVO_MAX_PULSEWIDTH_US; // maximum pulse width for this servo
    int pinNumber = -1;                // GPIO pin assigned to this channel
    uint8_t cus = 1500;                // Default Microseconds of the Servo (90 Degrees)
};

#else

class MCServo
{

public:
    MCServo();

    // Arduino Servo Library calls
    int attach(int pin);                   // attach the given pin to the next free channel, returns channel number or -1 if failure
    int attach(int pin, int min, int max); // as above but also sets min and max values for writes.
    void detach();
    void write(int value);             // Write pulse width in angles. If value > SERVO_MIN_PULSEWIDTH_US, the it is taken as mircoseconds (For Arduino compatibility. Why?)
    void writeMicroseconds(uint32_t value); // Write pulse width in microseconds
    void release();                    // Opposite of write, the servo stops holding the angle passed by write()
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    uint32_t readMicroseconds();            // returns current pulse width in microseconds for this servo
    bool attached();                   // return true if this servo is attached, otherwise false

private:
    int min;                     // minimum pulse width for this servo
    int max;                     // maximum pulse width for this servo
    int pinNumber;               // GPIO pin assigned to this channel
    uint32_t cus;                 // Default Microseconds of the Servo (90 Degrees)
    int unit;
    int signal;
    int timer;
    bool gInit;
};

#endif

#endif