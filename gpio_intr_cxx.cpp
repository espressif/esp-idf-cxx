/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstring>
#include <iostream>
#include "gpio_intr_cxx.hpp"
#include "driver/gpio.h"

namespace idf {

/**************************************************************************
 * GPIOIntr
 */

void GPIOIntr::set_type(const GPIOIntrType type)
{
    GPIO_CHECK_THROW(gpio_set_intr_type(gpio_num.get_value<gpio_num_t>(),
                                        type.get_value<gpio_int_type_t>()));
}

void GPIOIntr::driver_handler(void) {
    for (auto &cb: cb_table) {
        cb.second(gpio_num);
    }
}

void GPIOIntr::add_callback(std::string cb_name, interrupt_callback_t func_cb)
{
    if (cb_table.empty())
    {
        // first user callback registered, register driver_handler to the driver
        // by using the bounce function
        GPIO_CHECK_THROW(gpio_isr_handler_add(gpio_num.get_value<gpio_num_t>(),
                                        (gpio_isr_t)(&GPIOIntr::hdlr_bounce),
                                        this));
    }

    auto cb_it = find_user_callback(cb_name);
    if (cb_it == cb_table.end())
    {
        // add callback since it was not found
        cb_table.push_back({cb_name, func_cb});
    }
}

void GPIOIntr::remove_callback(std::string cb_name)
{
    auto cb_it = find_user_callback(cb_name);
    if (cb_it != cb_table.end())
    {
        // callback found, remove it
        cb_table.erase(cb_it);
    }

    if (cb_table.empty())
    {
        GPIO_CHECK_THROW(gpio_isr_handler_remove(gpio_num.get_value<gpio_num_t>()));
    }
}

void GPIOIntr::remove_all_callbacks()
{
    GPIO_CHECK_THROW(gpio_isr_handler_remove(gpio_num.get_value<gpio_num_t>()));
    cb_table.clear();
}

void GPIOIntr::enable_intr() const
{
    GPIO_CHECK_THROW(gpio_intr_enable(gpio_num.get_value<gpio_num_t>()));
}

void GPIOIntr::disable_intr() const
{
    GPIO_CHECK_THROW(gpio_intr_disable(gpio_num.get_value<gpio_num_t>()));
}

GPIOIntr::user_cb_table_t::iterator GPIOIntr::find_user_callback(std::string cb_name)
{
    return std::find_if(cb_table.begin(), cb_table.end(), [cb_name](cb_table_entry_t e) {
        return e.first == cb_name;
    });
}

void GPIOIntr::hdlr_bounce(void* class_ptr)
{
	((*reinterpret_cast<GPIOIntr*>(class_ptr)).driver_handler)();
}

/**************************************************************************
 * GPIOIsr
 */

GPIOIsr::GPIOIsr(): isr_service_started(false)
{
    // nothing to do here
}

void GPIOIsr::start_service(GPIOIsrFlag flag)
{   
    if (isr_service_started) { return; }
    GPIO_CHECK_THROW(gpio_install_isr_service(flag.get_value()));
    isr_service_started = true;
}

void GPIOIsr::stop_service(void)
{
    gpio_uninstall_isr_service();
    isr_service_started = false;
}

}  // namespace idf