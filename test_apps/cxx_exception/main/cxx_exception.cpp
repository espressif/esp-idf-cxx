/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0 OR Unlicense
 *
 * Exception-related unit tests
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <cstring>
#include "unity.h"
#include "utils_cxx.hpp"
#include "unity_test_utils_memory.h"

#include "unity_cxx.hpp"
#include "esp_exception.hpp"

using namespace std;
using namespace idf;

#define TAG "CXX Exception Test"

extern "C" void setUp()
{
    // The first exception thrown will allocate some memory, which is normal. 
    // Hence, we throw and catch an exception here to avoid reporting a memory leak on first test. 
    try {
        throw 47;
    } catch (int &e) { }
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

extern "C" void tearDown()
{
    unity_utils_evaluate_leaks();
}

TEST_CASE("TEST_THROW catches exception", "[exception_utils]")
{
    TEST_THROW(throw ESPException(ESP_FAIL);, ESPException);
}

TEST_CASE("CHECK_THROW continues on ESP_OK", "[exception_utils]")
{
    esp_err_t error = ESP_OK;
    CHECK_THROW(error);
}

TEST_CASE("CHECK_THROW throws", "[exception_utils]")
{
    esp_err_t error = ESP_FAIL;
    TEST_THROW(CHECK_THROW(error), ESPException);
}

TEST_CASE("ESPException has working what() method", "[exception_utils]")
{
    try {
        throw ESPException(ESP_FAIL);
    } catch (ESPException &e) {
        TEST_ASSERT(strcmp(esp_err_to_name(ESP_FAIL), e.what()) == 0);
    }
}

/* The following two test cases are expected to fail */

TEST_CASE("TEST_THROW asserts catching different exception", "[test_throw_fail]")
{
    TEST_THROW(throw std::exception();, ESPException);
}

TEST_CASE("TEST_THROW asserts not catching any exception", "[test_throw_fail]")
{
    TEST_THROW(printf(" ");, ESPException); // need statement with effect
}

extern "C" void app_main(void)
{
    TEST_APPS_LOG("CXX EXCEPTION TEST");
    unity_run_menu();
}
