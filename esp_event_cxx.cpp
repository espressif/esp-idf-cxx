/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_event_cxx.hpp"

#ifdef __cpp_exceptions

using namespace idf::event;
using namespace std;

namespace idf {

namespace event {

const std::chrono::milliseconds PLATFORM_MAX_DELAY_MS(portMAX_DELAY *portTICK_PERIOD_MS);

ESPEventReg::ESPEventReg(std::function<void(const ESPEvent &, void*)> cb,
        const ESPEvent& ev,
        std::shared_ptr<ESPEventAPI> api)
    : cb(cb), event(ev), api(api)
{
    if (!cb) throw EventException(ESP_ERR_INVALID_ARG);
    if (!api) throw EventException(ESP_ERR_INVALID_ARG);

    esp_err_t reg_result = api->handler_register(ev.base, ev.id.get_id(), event_handler_hook, this, &instance);
    if (reg_result != ESP_OK) {
        throw ESPEventRegisterException(reg_result, event);
    }
}

ESPEventReg::~ESPEventReg()
{
    api->handler_unregister(event.base, event.id.get_id(), instance);
}

void ESPEventReg::dispatch_event_handling(ESPEvent event, void *event_data)
{
    cb(event, event_data);
}

void ESPEventReg::event_handler_hook(void *handler_arg,
                                            esp_event_base_t event_base,
                                            int32_t event_id,
                                            void *event_data)
{
    ESPEventReg *object = static_cast<ESPEventReg*>(handler_arg);
    object->dispatch_event_handling(ESPEvent(event_base, ESPEventID(event_id)), event_data);
}

ESPEventRegTimed::ESPEventRegTimed(std::function<void(const ESPEvent &, void*)> cb,
        const ESPEvent& ev,
        std::function<void(const ESPEvent &)> timeout_cb,
        const std::chrono::microseconds &timeout,
        std::shared_ptr<ESPEventAPI> api)
    : ESPEventReg(cb, ev, api), timeout_cb(timeout_cb)
{
    if (!timeout_cb || timeout < MIN_TIMEOUT) {
        throw EventException(ESP_ERR_INVALID_ARG);
    }

    const esp_timer_create_args_t oneshot_timer_args {
        timer_cb_hook,
        static_cast<void*>(this),
        ESP_TIMER_TASK,
        "event",
        false // skip_unhandled_events
    };

    esp_err_t res = esp_timer_create(&oneshot_timer_args, &timer);
    if (res != ESP_OK) {
        throw EventException(res);
    }

    esp_err_t timer_result = esp_timer_start_once(timer, timeout.count());
    if (timer_result != ESP_OK) {
        esp_timer_delete(timer);
        throw EventException(timer_result);
    }
}

ESPEventRegTimed::~ESPEventRegTimed()
{
    std::lock_guard<mutex> guard(timeout_mutex);
    esp_timer_stop(timer);
    esp_timer_delete(timer);
    // TODO: is it guaranteed that there is no pending timer callback for timer?
}

void ESPEventRegTimed::dispatch_event_handling(ESPEvent event, void *event_data)
{
    if (timeout_mutex.try_lock()) {
        esp_timer_stop(timer);
        cb(event, event_data);
        timeout_mutex.unlock();
    }
}

void ESPEventRegTimed::timer_cb_hook(void *arg)
{
    ESPEventRegTimed *object = static_cast<ESPEventRegTimed *>(arg);
    if (object->timeout_mutex.try_lock()) {
        object->timeout_cb(object->event);
        object->api->handler_unregister(object->event.base, object->event.id.get_id(), object->instance);
        object->timeout_mutex.unlock();
    }
}

ESPEventLoop::ESPEventLoop(std::shared_ptr<ESPEventAPI> api) : api(api) {
    if (!api) throw EventException(ESP_ERR_INVALID_ARG);
}

ESPEventLoop::~ESPEventLoop() { }

unique_ptr<ESPEventReg> ESPEventLoop::register_event(const ESPEvent &event,
                                   function<void(const ESPEvent &, void*)> cb)
{
    return unique_ptr<ESPEventReg>(new ESPEventReg(cb, event, api));
}

std::unique_ptr<ESPEventRegTimed> ESPEventLoop::register_event_timed(const ESPEvent &event,
        std::function<void(const ESPEvent &, void*)> cb,
        const std::chrono::microseconds &timeout,
        std::function<void(const ESPEvent &)> timer_cb)
{
    return std::unique_ptr<ESPEventRegTimed>(new ESPEventRegTimed(cb, event, timer_cb, timeout, api));
}

void ESPEventLoop::post_event_data(const ESPEvent &event,
        const chrono::milliseconds &wait_time)
{
    esp_err_t result = api->post(event.base,
            event.id.get_id(),
            nullptr,
            0,
            convert_ms_to_ticks(wait_time));

    if (result != ESP_OK) {
        throw ESPException(result);
    }
}

TickType_t convert_ms_to_ticks(const std::chrono::milliseconds &time)
{
    return time.count() / portTICK_PERIOD_MS;
}

} // namespace event

} // namespace idf

#endif // __cpp_exceptions
