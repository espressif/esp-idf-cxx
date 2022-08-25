/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0 OR Unlicense
 */

#pragma once

#include "esp_log.h"

const char *TEST_APPS_TAG = "test_apps";

#define TEST_APPS_LOG(fmt, ...) esp_log_write(ESP_LOG_INFO, TEST_APPS_TAG, LOG_FORMAT(I, fmt), esp_log_timestamp(), TEST_APPS_TAG, ##__VA_ARGS__)
#define TEST_APPS_LOGW(fmt, ...) esp_log_write(ESP_LOG_WARN, TEST_APPS_TAG, LOG_FORMAT(W, fmt), esp_log_timestamp(), TEST_APPS_TAG, ##__VA_ARGS__)
#define TEST_APPS_LOGE(fmt, ...) esp_log_write(ESP_LOG_ERROR, TEST_APPS_TAG, LOG_FORMAT(E, fmt), esp_log_timestamp(), TEST_APPS_TAG, ##__VA_ARGS__)
