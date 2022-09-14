/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __cpp_exceptions

#include <cstdint>
#include <functional>
#include <list>
#include <string>

#include "gpio_cxx.hpp"

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

class GPIOIntr {
public:
    GPIOIntr(const GPIONum gpio_number): gpio_num(gpio_number)
    {
    }

    /**
     * @brief Callback footprint declaration for the
     * GPIO interrupt users.
     * @todo Define what will be useful to store in the parameter. For
     * now it is just the gpio_number but we might want to send more.
     */
    typedef std::function<void(GPIONum)> interrupt_callback_t;

    /**
     * @brief Set the interrupt type on the GPIO input
     * 
     * @note calling this function will overwrite the previously
     * set interrupt type if any.
     * 
     * @param type the interrupt type to be set
     */
    void set_type(const GPIOIntrType type);

    /**
     * @brief The callback to register to the driver
     * 
     * @param arg Argument passed in the callback, set when registering this callback
     */
    void driver_handler(void);

    /**
     * @brief Add a user callback in the list of registered callbacks
     * 
     * @param name The name of the callback
     * @param func_cb The user callback
     */
    void add_callback(std::string name, interrupt_callback_t func_cb);

    /**
     * @brief Remove a user callbacks based on its name in the list
     * 
     * @note If no callback is found, this method as no effect
     * 
     * @param cb_name the name of the callback to be removed
     */
    void remove_callback(std::string cb_name);

    /**
     * @brief remove all the user registered callbacks and unregister
     * the driver_handler to the gpio driver.
     */
    void remove_all_callbacks();

    /**
     * @brief Enable the interrupts on a given GPIO
     */
    void enable_intr() const;

    /**
     * @brief Disable interrupts on a given GPIO
     */
    void disable_intr() const;

    /**
     * @brief Function called form the gpio driver on interrupt with
     * the pointer to the appropriate instance of GPIOIntr 
     * as parameter.
     * 
     * @param class_ptr The pointer to the instance of GPIOIntr
     */
    static void hdlr_bounce(void* class_ptr);

    /**
     * @brief Utility method returning the gpio number of this instance of
     * GPIOIntr
     * 
     * @return const GPIONum& The gpio number associated to this instance of GPIOIntr
     */
    inline const GPIONum& get_gpio_number() const { return gpio_num; }

private:
    /**
     * @brief Pair of callback ID and callback function
     */
    typedef std::pair<std::string, interrupt_callback_t> cb_table_entry_t;

    /**
     * @brief List of callbacks registered by the user
     */
    typedef std::list<cb_table_entry_t> user_cb_table_t;

    /**
     * @brief List of registered user callbacks
     */
    user_cb_table_t cb_table;

    /**
     * @brief Reference to the input GPIO on which the interrupt is
     * defined.
     */
    GPIONum gpio_num;

    /**
     * @brief Returns the iterator in cb_table where the callback with the given name
     * was found, returns the end of the list if no callback was found
     * 
     * @param cb_name The name of the callback to be found 
     * @return std::iterator<user_cb_table_t> Iterator of the callback in cb_table
     */
    user_cb_table_t::iterator find_user_callback(std::string cb_name);
};

/**
 * @brief GPIO interrupt singleton. Used to register interrupt callback
 * on given GPIOs.
 */
class GPIOIsr {
public:
    /**
     * @brief Get the reference to the singleton
     *
     * @return GPIOIsr& Reference to this instance
     */
    static GPIOIsr& get_instance()
    {
        static GPIOIsr gpio_isr;
        return gpio_isr;
    }

    /**
     * @brief Start ISR service with the given set of flags
     *
     * @param flag ORed flag to be set when starting the ISR service.
     */
    void start_service(GPIOIsrFlag flag);

    /**
     * @brief Stop the ISR service
     */
    void stop_service(void);

private:

    /**
     * @brief Hold information about whether or not the underlying gpio isr service
     * was started.
     */
    bool isr_service_started;

    GPIOIsr();
    GPIOIsr(const GPIOIsr&);
    GPIOIsr& operator=(const GPIOIsr&);
};

/**
 * @brief Directly accessible by user to avoid calling GPIOIsr::get_instance() all the time.
 */
static GPIOIsr &GPIO_Isr = GPIOIsr::get_instance();

} // namespace idf

#endif // __cpp_exceptions