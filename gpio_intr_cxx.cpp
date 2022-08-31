/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstring>

#include "gpio_intr_cxx.hpp"
#include "driver/gpio.h"

namespace idf {

namespace {

/**
 * @brief Return the converted GPIO interrupt type usable by the C driver
 * 
 * @param num The CXX representation of the GPIO interrupt type
 * @return gpio_num_t The C driver representation of the GPIO interrupt type
 */
gpio_int_type_t gpio_int_type_to_driver_type(GPIOIntrType type) {
    return static_cast<gpio_int_type_t>(type.get_value());
}

/**
 * @brief Return the converted GPIO number usable by the C driver
 * 
 * @param num The CXX representation of the GPIO number
 * @return gpio_num_t The C driver representation of the GPIO number
 */
gpio_num_t gpio_num_to_driver_num(GPIONum num) {
    return static_cast<gpio_num_t>(num.get_value());
}

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
        cb->second(GPIONum(gpio_number));
    }
}
} // namespace

void GPIO_Intr::gpio_start_isr_service(GPIOIsrFlag flag)
{
    if (isr_service_started) { return; }
    GPIO_CHECK_THROW(gpio_install_isr_service(flag.get_value()));
    isr_service_started = true;
}

void GPIO_Intr::gpio_stop_isr_service(void)
{
    gpio_uninstall_isr_service();
    isr_service_started = false;
}

void GPIO_Intr::gpio_set_intr(GPIONum gpio_number, GPIOIntrType type, interrupt_callback func_cb)
{
    GPIO_CHECK_THROW(isr_service_started == true ? ESP_OK : ESP_FAIL);
    
    if (callback_table.find(gpio_number.get_num()) == callback_table.end())
    {
        callback_table.insert({gpio_number.get_num(), func_cb});

        GPIO_CHECK_THROW(gpio_set_intr_type(gpio_num_to_driver_num(gpio_number),
                                            gpio_int_type_to_driver_type(type)));

        GPIO_CHECK_THROW(gpio_isr_handler_add(gpio_num_to_driver_num(gpio_number),
                                              static_cast<gpio_isr_t>(generic_callback),
                                              reinterpret_cast<void*>(gpio_number.get_num())));
    }
}

void GPIO_Intr::gpio_enable_intr(GPIONum gpio_number) const
{
    GPIO_CHECK_THROW(gpio_intr_enable(gpio_num_to_driver_num(gpio_number)));
}

void GPIO_Intr::gpio_disable_intr(GPIONum gpio_number) const
{
    GPIO_CHECK_THROW(gpio_intr_disable(gpio_num_to_driver_num(gpio_number)));
}

}  // namespace idf