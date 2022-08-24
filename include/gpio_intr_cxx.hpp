/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>

namespace idf
{
/**
 * @brief GPIO interrupt singleton. Used to register interrupt callback
 * on given GPIOs.
 */
class GPIO_Intr {
public:
    /**
     * @brief Callback footprint declaration for the
     * GPIO interrupt users.
     * @todo Define what will be useful to store in the parameter. For
     * now it is just the gpio_number but we might want to send more.
     */
    typedef std::function<void(uint32_t)> interrupt_callback;

    /**
     * @brief CXX GPIO interrupt types
     * @todo Link with C definitions
     */
    typedef uint32_t gpio_intr_type;

    /**
     * @brief CXX GPIO interrupt priority
     * @todo Link with C definitions
     */
    typedef uint32_t gpio_intr_priority;

    /**
     * @brief Get the reference to the singleton
     * 
     * @return GPIO_Intr& Reference to this instance
     */
    static GPIO_Intr& get_instance()
    {
        static GPIO_Intr gpio_intr;
        return gpio_intr;
    }

    /**
     * @brief Set an interrupt of a given type oin a given GPIO. Register the function callback
     * linked to this interrupt.
     * 
     * @param gpio_input The GPIO number
     * @param type The type of interrupt that is triggered on the GPIO
     * @param priority The priority of the interrupt registered
     * @param func_cb The callback function called on interrupt
     * @return true The registration was successful
     * @return false Wrong GPIO, wrong interrupt type or null pointer passed as arguments.
     */
    bool gpio_intr_set(uint32_t gpio_number, gpio_intr_type type, gpio_intr_priority priority, interrupt_callback func_cb);

    /**
     * @brief Enable the interrupts on a given GPIO
     * 
     * @param gpio_input The GPIO number
     */
    void gpio_intr_enable(uint32_t gpio_number);

    /**
     * @brief Disable interrupts on a given GPIO
     * 
     * @param gpio_input The GPIO number
     */
    void gpio_intr_disable(uint32_t gpio_number);

    /**
     * @brief Get the callback table to find what user defined callback to call
     * from the generic callback function defined static in gpio_intr_cxx.cpp
     * 
     * @todo Find a way to pass the user callback directly to the gpio driver?
     * 
     * @return std::map<uint32_t, interrupt_callback>& The callback map addressed by GPIO number
     */
    inline std::map<uint32_t, interrupt_callback>& get_table() {return callback_table; }

private:

    /**
     * @brief Hold information about whether or not the underlying gpio isr service
     * was started.
     * 
     * @todo Find out more about this gpio_install_isr_service()
     */
    bool isr_service_started;

    /**
     * @brief Map of user callback associated to GPIO number
     *  
     * @todo Change to a map of list? If several callbacks are defined for one GPIO?
     */
    std::map<uint32_t, interrupt_callback> callback_table;

    GPIO_Intr(): isr_service_started(false) {}
    GPIO_Intr(const GPIO_Intr&);
    GPIO_Intr& operator=(const GPIO_Intr&);
};

/**
 * @brief Directly accessible by user to avoid calling GPIO_intr::get_instance() all the time.
 */
static GPIO_Intr &GPIOIntr = GPIO_Intr::get_instance();

} // namespace idf
