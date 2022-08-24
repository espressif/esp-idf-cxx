/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstring>

#include "gpio_intr_cxx.hpp"
#include "driver/gpio.h"

namespace idf {
/**
 * @brief Generic callback registered to the gpio driver
 * 
 * @todo Remove this hack.
 * 
 * @param arg Represents the GPIO number triggering the interrupt 
 */
static void generic_callback(void *arg)
{
    GPIO_Intr &gpio_intr = GPIO_Intr::get_instance();
    auto table = gpio_intr.get_table();
    // find the c++ user callback create the parameter and call it.

    /**
     * @note casting to unsigned 64 bits to avoid the compiler error about the loss of precision
     * @todo remove this hack
     */
    uint32_t gpio_number = reinterpret_cast<uint64_t>(arg);
    
    auto cb = table.find(gpio_number);
    if (cb != table.end())
    {
        cb->second(gpio_number);
    }
}

bool GPIO_Intr::gpio_intr_set(uint32_t gpio_number, gpio_intr_type type, gpio_intr_priority priority, interrupt_callback func_cb)
{
    esp_err_t ret_val = ESP_FAIL;

    if (isr_service_started != true)
    {
        gpio_install_isr_service(0);
        isr_service_started = true;
    }

    if (callback_table.find(gpio_number) == callback_table.end())
    {
        callback_table.insert({gpio_number, func_cb});
        gpio_set_intr_type((gpio_num_t) gpio_number, GPIO_INTR_POSEDGE);

        ret_val = gpio_isr_handler_add(static_cast<gpio_num_t>(gpio_number),
                                        static_cast<gpio_isr_t>(generic_callback),
                                        reinterpret_cast<void*>(gpio_number));
    }
    
    if (ret_val == ESP_OK)
    {
        printf("interrupt handler added successfully\n");
    }

    return (ret_val == ESP_OK ? true : false);
}

void GPIO_Intr::gpio_intr_enable(uint32_t gpio_number)
{
    gpio_intr_enable(gpio_number);
}

void GPIO_Intr::gpio_intr_disable(uint32_t gpio_number)
{
    gpio_intr_disable(gpio_number);
}

}  // namespace idf