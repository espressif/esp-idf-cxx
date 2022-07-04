/*
 * SPDX-FileCopyrightText: 2020 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cpp_exceptions

#include <functional>
#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"

using namespace std;

namespace idf {

namespace esp_timer {

ESPTimer::ESPTimer(function<void()> timeout_cb, const string &timer_name)
    : timeout_cb(timeout_cb), name(timer_name)
{
    if (timeout_cb == nullptr) {
        throw ESPException(ESP_ERR_INVALID_ARG);
    }

    esp_timer_create_args_t timer_args = {};
    timer_args.callback = esp_timer_cb;
    timer_args.arg = this;
    timer_args.dispatch_method = ESP_TIMER_TASK;
    timer_args.name = name.c_str();

    CHECK_THROW(esp_timer_create(&timer_args, &timer_handle));
}

ESPTimer::~ESPTimer()
{
    // Ignore potential ESP_ERR_INVALID_STATE here to not throw exception.
    esp_timer_stop(timer_handle);
    esp_timer_delete(timer_handle);
}

void ESPTimer::esp_timer_cb(void *arg)
{
    ESPTimer *timer = static_cast<ESPTimer*>(arg);
    timer->timeout_cb();
}

} // esp_timer

} // idf

#endif // __cpp_exceptions
