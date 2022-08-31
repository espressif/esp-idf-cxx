/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __cpp_exceptions

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>

#include "gpio_cxx.hpp"

using namespace std;

namespace idf {

/**
 * @brief Valid representation of ISR service flags.
 */
class GPIOIsrFlag {
public:
    constexpr explicit GPIOIsrFlag() : flags(0) { }

    /**
     * @brief Add the LEVEL1 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL1() {
        add_flag(1<<1);
        return *this;
    }

    /**
     * @brief Add the LEVEL2 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL2() {
        add_flag(1<<2);
        return *this;
    }

    /**
     * @brief Add the LEVEL3 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL3() {
        add_flag(1<<3);
        return *this;
    }

    /**
     * @brief Add the LEVEL4 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL4() {
        add_flag(1<<4);
        return *this;
    }

    /**
     * @brief Add the LEVEL5 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL5() {
        add_flag(1<<5);
        return *this;
    }

    /**
     * @brief Add the LEVEL6 by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& LEVEL6() {
        add_flag(1<<6);
        return *this;
    }

    /**
     * @brief Add the NMI by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& NMI() {
        add_flag(1<<7);
        return *this;
    }

    /**
     * @brief Add the SHARED by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& SHARED() {
        add_flag(1<<8);
        return *this;
    }

    /**
     * @brief Add the EDGE by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& EDGE() {
        add_flag(1<<9);
        return *this;
    }

    /**
     * @brief Add the IRAM by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& IRAM() {
        add_flag(1<<10);
        return *this;
    }

    /**
     * @brief Add the INTRDISABLED by OR operation to the
     * member variable flags
     * 
     * @return GPIOIsrFlag& Return a reference to the class
     * to allow concatenating calls to other functions.
     */
    constexpr GPIOIsrFlag& INTRDISABLED() {
        add_flag(1<<11);
        return *this;
    }

    /**
     * @brief Get the flags value
     * 
     * @return int The ISR service flag
     */
    inline int get_value() const {
        return flags;
    }

private:
    int flags;

    inline constexpr void add_flag(int flag) {
        flags |= flag;
    }

};

/**
 * @brief Valid representation of GPIO interrupt type.
 */
class GPIOIntrType : public StrongValueComparable<uint8_t> {
    /**
     * Construct a valid representation of the GPIO interrupt type.
     *
     * This constructor is private because the it can only be accessed but the static creation methods below.
     * This guarantees that an instance of GPIOIntrType always carries a valid number.
     */
    constexpr explicit GPIOIntrType(uint8_t number) : StrongValueComparable<uint8_t>(number) { }

public:
    /**
     * @brief create a GPIO interrupt type DISABLE.
     */
    constexpr static GPIOIntrType DISABLE() {
        return GPIOIntrType(0);
    }

    /**
     * @brief create a GPIO interrupt type POSEDGE.
     */
    constexpr static GPIOIntrType POSEDGE() {
        return GPIOIntrType(1);
    }

    /**
     * @brief create a GPIO interrupt type NEGEDGE.
     */
    constexpr static GPIOIntrType NEGEDGE() {
        return GPIOIntrType(2);
    }

    /**
     * @brief create a GPIO interrupt type ANYEDGE.
     */
    constexpr static GPIOIntrType ANYEDGE() {
        return GPIOIntrType(3);
    }

    /**
     * @brief create a GPIO interrupt type LOW_LEVEL.
     */
    constexpr static GPIOIntrType LOW_LEVEL() {
        return GPIOIntrType(4);
    }

    /**
     * @brief create a GPIO interrupt type HIGH_LEVEL.
     */
    constexpr static GPIOIntrType HIGH_LEVEL() {
        return GPIOIntrType(5);
    }
};

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
    typedef function<void(GPIONum)> interrupt_callback;

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
     * @brief Start ISR service with the given set of flags
     * 
     * @param flag ORed flag to be set when starting the ISR service.
     */
    void gpio_start_isr_service(GPIOIsrFlag flag);

    /**
     * @brief Stop the ISR service
     */
    void gpio_stop_isr_service(void);

    /**
     * @brief Set an interrupt of a given type oin a given GPIO. Register the function callback
     * linked to this interrupt.
     * 
     * @param gpio_input The GPIO number
     * @param type The type of interrupt that is triggered on the GPIO
     * @param func_cb The callback function called on interrupt
     */
    void gpio_set_intr(GPIONum gpio_number, GPIOIntrType type, interrupt_callback func_cb);

    /**
     * @brief Disable interrupt on the given GPIO number and remove the associated handler
     * 
     * @param gpio_number The GPIO number on which to remove the interrupt service
     */
    void gpio_remove_intr(GPIONum gpio_number);

    /**
     * @brief Enable the interrupts on a given GPIO
     * 
     * @param gpio_input The GPIO number
     */
    void gpio_enable_intr(GPIONum gpio_number) const;

    /**
     * @brief Disable interrupts on a given GPIO
     * 
     * @param gpio_input The GPIO number
     */
    void gpio_disable_intr(GPIONum gpio_number) const;

    /**
     * @brief Get the callback table to find what user defined callback to call
     * from the generic callback function defined static in gpio_intr_cxx.cpp
     * 
     * @todo Find a way to pass the user callback directly to the gpio driver?
     * 
     * @return map<uint32_t, interrupt_callback>& The callback map addressed by GPIO number
     */
    inline map<uint32_t, interrupt_callback>& get_table() {return callback_table; }

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
    map<uint32_t, interrupt_callback> callback_table;

    GPIO_Intr(): isr_service_started(false) {}
    GPIO_Intr(const GPIO_Intr&);
    GPIO_Intr& operator=(const GPIO_Intr&);
};

/**
 * @brief Directly accessible by user to avoid calling GPIO_intr::get_instance() all the time.
 */
static GPIO_Intr &GPIOIntr = GPIO_Intr::get_instance();

} // namespace idf

#endif // __cpp_exceptions