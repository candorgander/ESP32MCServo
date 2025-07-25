#ifndef MCPWM_CPP
#define MCPWM_CPP

#include "ESP32MCPWM.h"
#include "ArduinoTrace.h"

static const char *TAG = "ESP32MCPWM";

ESP32MCPWM::ESP32MCPWM()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    // // TRACE();

    allocated = 0;
    for (int i = 0; i < MCPWM_UNIT_NUM; i++)
    {
        for (int j = 0; j < MCPWM_SIGNAL_NUM; j++)
        {
            mc_channels[i][j] = false;
            // ESP_LOGI(TAG, "i: %d, j: %d", i, j);
        }
    }
    ESP_LOGD(TAG, "ESP32PWM Init success");
    init = true;
    // TRACE();
#endif
}

// Return the number of channels available
int ESP32MCPWM::getAvailable()
{
    return MCPWM_TOTAL - allocated;
}

// Acquire the first available channel
mcpwm_data ESP32MCPWM::getChannel()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    if (allocated <= MCPWM_TOTAL)
    {
        for (int i = 0; i < MCPWM_UNIT_NUM; i++)
        {
            for (int j = 0; j < MCPWM_SIGNAL_NUM; j++)
            {
                if (mc_channels[i][j] == false)
                {
                    ESP_LOGV(TAG, "Allocating channel %d from unit %d", j, i);
                    mcpwm_data d = {
                        .unit = i,
                        .signal = j};

                    mc_channels[i][j] = true;
                    allocated += 1;

                    return d;
                }
            }
        }
    }
    else
    {
        ESP_LOGE(TAG, "No channels available");
        // Return error struct
        mcpwm_data d = {
            .unit = -1,
            .signal = -1};

        return d;
    }
#endif
}

// Release the passed channel
bool ESP32MCPWM::releaseChannel(mcpwm_data c, mcpwm_timer_t t)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // Handle Arduino 3.x
    ESP_LOGE(TAG, "Support for Arduino 3.x/IDF 5.x is pending");
#else
    if (c.unit != -1 && c.signal != -1)
    {
        ESP_LOGV(TAG, "Releasing channel %d of unit %d", c.signal, c.unit);

        allocated -= 1;
        mc_channels[c.unit][c.signal] = false;

        mcpwm_stop((mcpwm_unit_t)c.unit, t);

        return true;
    }
    else
    {
        ESP_LOGE(TAG, "Releasing unitialized Channel!");
        return false;
    }
#endif
}

#endif