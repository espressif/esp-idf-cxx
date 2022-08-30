/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0 OR Unlicense
 *
 * I2C C++ unit tests
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <limits>
#include <stdio.h>
#include <iostream>

#include "unity.h"
#include "unity_cxx.hpp"
#include "test_utils.h" // ref clock

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "memory_checks.h"

#include "esp_exception.hpp"
#include "esp_timer_cxx.hpp"

using namespace std;
using namespace idf;
using namespace idf::esp_timer;

constexpr size_t LEAKS = 250;

extern "C" void setUp()
{
    test_utils_record_free_mem();
}

extern "C" void tearDown()
{
    test_utils_finish_and_evaluate_leaks(LEAKS, LEAKS);
}

struct RefClock {
    RefClock()
    {
        ref_clock_init();
    };

    ~RefClock()
    {
        ref_clock_deinit();
    }
};

TEST_CASE("ESPTimer produces correct delay", "[ESPTimer]")
{
    int64_t t_end;

    RefClock ref_clock;

    function<void()> timer_cb = [&t_end]() {
        t_end = ref_clock_get();
    };

    ESPTimer timer(timer_cb, "timer1");

    const int delays_ms[] = {20, 100, 200, 250};
    const size_t delays_count = sizeof(delays_ms)/sizeof(delays_ms[0]);

    for (size_t i = 0; i < delays_count; ++i) {
        t_end = 0;
        int64_t t_start = ref_clock_get();

        timer.start(chrono::microseconds(delays_ms[i] * 1000));

        vTaskDelay(delays_ms[i] * 2 / portTICK_PERIOD_MS);
        TEST_ASSERT(t_end != 0);
        int32_t ms_diff = (t_end - t_start) / 1000;
        printf("%d %ld\n", delays_ms[i], ms_diff);

        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, delays_ms[i], ms_diff);
    }
}

TEST_CASE("ESPtimer produces correct periodic delays", "[ESPTimer]")
{
    const size_t NUM_INTERVALS = 3u;

    size_t cur_interval = 0;
    int intervals[NUM_INTERVALS];
    int64_t t_start;
    SemaphoreHandle_t done;

    const int DELAY_MS = 100;
    function<void()> timer_cb = [&]() {
        int64_t t_end = ref_clock_get();
        int32_t ms_diff = (t_end - t_start) / 1000;
        printf("timer #%d %ldms\n", cur_interval, ms_diff);
        if (cur_interval < NUM_INTERVALS) {
           intervals[cur_interval++] = ms_diff;
        }
        // Deliberately make timer handler run longer.
        // We check that this doesn't affect the result.
        esp_rom_delay_us(10*1000);
        if (cur_interval == NUM_INTERVALS) {
            printf("done\n");
            xSemaphoreGive(done);
        }
    };

    ESPTimer timer(timer_cb, "timer1");
    RefClock ref_clock;
    t_start = ref_clock_get();
    done = xSemaphoreCreateBinary();
    timer.start_periodic(chrono::microseconds(DELAY_MS * 1000));

    TEST_ASSERT(xSemaphoreTake(done, DELAY_MS * NUM_INTERVALS * 2));
    timer.stop();

    TEST_ASSERT_EQUAL_UINT32(NUM_INTERVALS, cur_interval);
    for (size_t i = 0; i < NUM_INTERVALS; ++i) {
        TEST_ASSERT_INT32_WITHIN(portTICK_PERIOD_MS, (i + 1) * DELAY_MS, intervals[i]);
    }
    TEST_ESP_OK(esp_timer_dump(stdout));

    vSemaphoreDelete(done);
}

extern "C" void app_main(void)
{
    printf("CXX ESP TIMER TEST\n");
    unity_run_menu();
}
