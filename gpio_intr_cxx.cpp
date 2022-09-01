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
    auto cbs = table.find(gpio_number);
    if (cbs != table.end())
    {
        for (auto &cb: cbs->second) {
            cb.second(GPIONum(gpio_number));
        }
    }
}
} // namespace

GPIO_Intr::GPIO_Intr(): isr_service_started(false), 
                        interrupt_type_set(false)
{
    // nothing to do here
}

void GPIO_Intr::start_isr_service(GPIOIsrFlag flag)
{
    if (isr_service_started) { return; }
    GPIO_CHECK_THROW(gpio_install_isr_service(flag.get_value()));
    isr_service_started = true;
}

void GPIO_Intr::stop_isr_service(void)
{
    gpio_uninstall_isr_service();
    isr_service_started = false;
}

void GPIO_Intr::set_type(GPIONum gpio_number, GPIOIntrType type)
{
    GPIO_CHECK_THROW(gpio_set_intr_type(gpio_num_to_driver_num(gpio_number),
                                        gpio_int_type_to_driver_type(type)));
    interrupt_type_set = true;
}

void GPIO_Intr::add_callback(GPIONum gpio_number,  std::string cb_name, interrupt_callback func_cb)
{
    GPIO_CHECK_THROW(isr_service_started == true && interrupt_type_set == true ? ESP_OK : ESP_FAIL);

    auto cbs = cb_table.find(gpio_number.get_num());
    if (cbs == cb_table.end())
    {
        cb_table.insert({gpio_number.get_num(), {cb_table_entry_t(cb_name, func_cb)}});

        // first callback registered, add the generic callback to the gpio driver
        GPIO_CHECK_THROW(gpio_isr_handler_add(gpio_num_to_driver_num(gpio_number),
                                              static_cast<gpio_isr_t>(generic_callback),
                                              reinterpret_cast<void*>(gpio_number.get_num())));
    }
    else if (find_if(cbs->second.begin(), cbs->second.end(), [cb_name](cb_table_entry_t e) {
        return cb_name == e.first;
    }) == cbs->second.end())
    {
        // generic callback already registered. Only add the user callback to the
        // list of callbacks associated to the given GPIO.
        cbs->second.push_back(cb_table_entry_t(cb_name, func_cb));
    }
    else
    {
        // nothing to do here
    }
}

void GPIO_Intr::remove_callback(GPIONum gpio_number, std::string cb_name)
{
    auto num = gpio_number.get_num();
    auto cbs = cb_table.find(num);
    GPIO_CHECK_THROW(cbs != cb_table.end() ? ESP_OK : ESP_FAIL);

    auto cb_it = find_if(cbs->second.begin(), cbs->second.end(), [cb_name](cb_table_entry_t e) {
        return cb_name == e.first;
    });
    if (cb_it != cbs->second.end())
    {
        cbs->second.erase(cb_it);
    }
       
    if (cbs->second.empty())
    {
        // if only one callback associated to the GPIO, remove the map entry
        cb_table.erase(num);

        // also de-register the generic callback from the driver
        GPIO_CHECK_THROW(gpio_isr_handler_remove(gpio_num_to_driver_num(gpio_number)));
    }
}

void GPIO_Intr::enable_intr(GPIONum gpio_number) const
{
    GPIO_CHECK_THROW(gpio_intr_enable(gpio_num_to_driver_num(gpio_number)));
}

void GPIO_Intr::disable_intr(GPIONum gpio_number) const
{
    GPIO_CHECK_THROW(gpio_intr_disable(gpio_num_to_driver_num(gpio_number)));
}

}  // namespace idf