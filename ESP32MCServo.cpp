#ifndef ESP32_MCPWM_SERVO_CPP
#define ESP32_MCPWM_SERVO_CPP

#include "ESP32MCServo.h"
#include "esp_log.h"
#include "ArduinoTrace.h"

ESP32MCPWM pwm;
static const char *TAG = "ESP32MCServo";

#ifdef ARDUINO_SERVO
#else
MCServo::MCServo()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    // TRACE();
    this->min = SERVO_MIN_PULSEWIDTH_US; // minimum pulse width for this servo
    this->max = SERVO_MAX_PULSEWIDTH_US; // maximum pulse width for this servo
    this->pinNumber = -1;                // GPIO pin assigned to this channel
    this->cus = 1500;                    // Default Microseconds of the Servo (90 Degrees)
    this->unit = -1;
    this->signal = -1;
    this->timer = -1;
    this->gInit = false;

    ESP_LOGI(TAG, "Servo init success");
    // TRACE();
#endif
}

// attach the given pin to the next free channel, returns channel number or -1 if failure
int MCServo::attach(int pin)
{
    return this->attach(pin, SERVO_MIN_PULSEWIDTH_US, SERVO_MAX_PULSEWIDTH_US);
}

// as above but also sets min and max values for writes.
int MCServo::attach(int pin, int min, int max)
{
    this->min = min;
    this->max = max;
    this->pinNumber = pin;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    while (!pwm.init)
    {
        vTaskDelay(10);
    }
    if (pwm.getAvailable())
    {
        mcpwm_data md = pwm.getChannel();
        this->unit = md.unit;
        this->signal = md.signal;

        if (md.unit == -1 || md.signal == -1)
        {
            ESP_LOGE(TAG, "No servo's available");
            return -1;
        }
        // TRACE();
        if ((mcpwm_gpio_init((mcpwm_unit_t)this->unit, (mcpwm_io_signals_t)this->signal, this->pinNumber)) != ESP_OK)
        {
            // TRACE();
            ESP_LOGE(TAG, "Error pin Init");
            return -1;
        }
        // TRACE();

        ESP_LOGI(TAG, "Pin %d is attached to MCPWM unit %d and signal %d", this->pinNumber, this->unit, this->signal);

        mcpwm_config_t pwm_config = {
            .frequency = 50, // frequency = 50Hz, i.e. for every servo motor time period should be 20ms
            .cmpr_a = 0,     // duty cycle of PWMxA = 0
            .cmpr_b = 0,     // duty cycle of PWMxB = 0
            .duty_mode = MCPWM_DUTY_MODE_0,
            .counter_mode = MCPWM_UP_COUNTER,

        };
        if (this->signal < 2)
        {
            ESP_ERROR_CHECK(mcpwm_init((mcpwm_unit_t)this->unit, MCPWM_TIMER_0, &pwm_config));
            this->timer = 0;
        }
        else if (this->signal == 2 || this->signal == 3)
        {
            ESP_ERROR_CHECK(mcpwm_init((mcpwm_unit_t)this->unit, MCPWM_TIMER_1, &pwm_config));
            this->timer = 1;
        }
        else if (this->signal == 4 || this->signal == 5)
        {
            ESP_ERROR_CHECK(mcpwm_init((mcpwm_unit_t)this->unit, MCPWM_TIMER_2, &pwm_config));
            this->timer = 2;
        }
        else
        {
            // We should not be here!!
            ESP_LOGE(TAG, "We should not be here!!");
        }

        mcpwm_set_duty_type((mcpwm_unit_t)this->unit, (mcpwm_timer_t)this->timer, (mcpwm_generator_t)(this->signal % 2), MCPWM_DUTY_MODE_0);
        this->gInit = true;
        ESP_LOGD(TAG, "ESP32Servo Init success");
        ESP_LOGD(TAG, "Servo pins left: %d", pwm.getAvailable());
        vTaskDelay(10);
        this->writeMicroseconds(this->cus);
        return this->signal;
    }
    else
    {
        ESP_LOGE(TAG, "No MCPWM channels available!!");
        this->gInit = false;
        return -1;
    }
#endif
}

void MCServo::detach()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    ESP_LOGI(TAG, "Detaching channel: %d of unit %d", this->signal, this->unit);

    mcpwm_data d{
        .unit = this->unit,
        .signal = this->signal};

    if (d.unit != -1 && d.signal != -1 && this->gInit)
    {
        pwm.releaseChannel(d, (mcpwm_timer_t)this->timer);
    }
    else
    {
        ESP_LOGE(TAG, "Tried to detach bad unit and signal or uninited servo!!");
    }
    ESP_LOGD(TAG, "Servo pins left: %d", pwm.getAvailable());

#endif
}

// Write pulse width in angles. If value > SERVO_MIN_PULSEWIDTH_US, the it is taken as mircoseconds (For Arduino compatibility. Why?)
void MCServo::write(int value)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    if (value < this->min)
    {
        this->writeMicroseconds(map(value, 0, 180, this->min, this->max));
    }
    else
    {
        this->writeMicroseconds(value);
    }

#endif
}

// Write pulse width in microseconds
void MCServo::writeMicroseconds(uint32_t value)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    this->cus = value;
    ESP_LOGD(TAG, "Wrote value: %u to unit: %d with timer: %d and generator: %d", value, this->unit, this->timer, this->signal % 2);
    ESP_LOGD(TAG, "Wrote value: %u to Servo Attached to pin %d", value, this->pinNumber);
    ESP_ERROR_CHECK(mcpwm_set_duty_in_us((mcpwm_unit_t)this->unit, (mcpwm_timer_t)this->timer, (mcpwm_generator_t)(this->signal % 2), value));
    // vTaskDelay(50);
#endif
}

// Opposite of write, the servo stops holding the angle passed by write()
void MCServo::release()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
#endif
}

// returns current pulse width as an angle between 0 and 180 degrees
int MCServo::read()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    return map(this->cus, this->min, this->max, 0, 180);
#endif
}

// returns current pulse width in microseconds for this servo
uint32_t MCServo::readMicroseconds()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    return this->cus;
#endif
}

// return true if this servo is attached, otherwise false
bool MCServo::attached()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    return gInit;
#endif
}
#endif

#endif