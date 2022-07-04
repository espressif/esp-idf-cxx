/*
 * SPDX-FileCopyrightText: 2020 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cpp_exceptions

#include <chrono>
#include <functional>
#include <string>
#include "esp_exception.hpp"
#include "esp_timer.h"

namespace idf {

namespace esp_timer {

/**
 * @brief Get time since boot
 * @return time since \c esp_timer_init() was called (this normally happens early during application startup).
 */
static inline std::chrono::microseconds get_time()
{
    return std::chrono::microseconds(esp_timer_get_time());
}

/**
 * @brief Get the timestamp when the next timeout is expected to occur
 * @return Timestamp of the nearest timer event.
 *         The timebase is the same as for the values returned by \c get_time().
 */
static inline std::chrono::microseconds get_next_alarm()
{
    return std::chrono::microseconds(esp_timer_get_next_alarm());
}


/**
 * @brief
 * A timer using the esp_timer component which can be started either as one-shot timer or periodically.
 */
class ESPTimer {
public:
    /**
     * @param timeout_cb The timeout callback.
     * @param timer_name The name of the timer (optional). This is for debugging using \c esp_timer_dump().
     */
    ESPTimer(std::function<void()> timeout_cb, const std::string &timer_name = "ESPTimer");

    /**
     * Stop the timer if necessary and delete it.
     */
    ~ESPTimer();

    /**
     * Default copy constructor is deleted since one instance of esp_timer_handle_t must not be shared.
     */
    ESPTimer(const ESPTimer&) = delete;

    /**
     * Default copy assignment is deleted since one instance of esp_timer_handle_t must not be shared.
     */
    ESPTimer &operator=(const ESPTimer&) = delete;

    /**
     * @brief Start one-shot timer
     *
     * Timer should not be running (started) when this function is called.
     *
     * @param timeout timer timeout, in microseconds relative to the current moment.
     *
     * @throws ESPException with error ESP_ERR_INVALID_STATE if the timer is already running.
     */
    inline void start(std::chrono::microseconds timeout)
    {
        CHECK_THROW(esp_timer_start_once(timer_handle, timeout.count()));
    }

    /**
     * @brief Start periodic timer
     *
     * Timer should not be running when this function is called. This function will
     * start a timer which will trigger every 'period' microseconds.
     *
     * Timer should not be running (started) when this function is called.
     *
     * @param timeout timer timeout, in microseconds relative to the current moment.
     *
     * @throws ESPException with error ESP_ERR_INVALID_STATE if the timer is already running.
     */
    inline void start_periodic(std::chrono::microseconds period)
    {
        CHECK_THROW(esp_timer_start_periodic(timer_handle, period.count()));
    }

    /**
     * @brief Stop the previously started timer.
     *
     * This function stops the timer previously started using \c start() or \c start_periodic().
     *
     * @throws ESPException with error ESP_ERR_INVALID_STATE if the timer has not been started yet.
     */
    inline void stop()
    {
        CHECK_THROW(esp_timer_stop(timer_handle));
    }

private:
    /**
     * Internal callback to hook into esp_timer component.
     */
    static void esp_timer_cb(void *arg);

    /**
     * Timer instance of the underlying esp_event component.
     */
    esp_timer_handle_t timer_handle;

    /**
     * Callback which will be called once the timer triggers.
     */
    std::function<void()> timeout_cb;

    /**
     * Name of the timer, will be passed to the underlying timer framework and is used for debugging.
     */
    const std::string name;
};

} // esp_timer

} // idf

#endif // __cpp_exceptions
