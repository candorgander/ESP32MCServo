#ifndef ESP32_MCPWM_H
#define ESP32_MCPWM_H
#if defined(ARDUINO)
#include <Arduino.h>
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
#include "driver/mcpwm.h"
#endif

#define MCPWM_UNIT_NUM 2
#define MCPWM_SIGNAL_NUM 6
#define MCPWM_TOTAL 12

struct mcpwm_data
{
    int unit;
    int signal;
};

class ESP32MCPWM
{
public:
    bool init;
    int getAvailable();
    mcpwm_data getChannel();
    bool releaseChannel(mcpwm_data c, mcpwm_timer_t t);
    ESP32MCPWM();

private:
    bool mc_channels[2][6];
    int allocated;
};

// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
// // Handle Arduino 3.x
// ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
// #else
// // Handle Arduino 2.x
// #endif

#endif