/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * MPU9250 SPI Sensor C++ Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <thread>
#include "spi_host_cxx.hpp"

using namespace std;
using namespace idf;

static const uint8_t READ_FLAG = 0x80;
static const uint8_t MPU9250_WHO_AM_I_REG_ADDR = 0x75;

namespace {
static const MOSI MOSI_PIN(CONFIG_SPI_MOSI);
static const MISO MISO_PIN(CONFIG_SPI_MISO);
static const SCLK SCLK_PIN(CONFIG_SPI_SCLK);
static const CS     CS_PIN(CONFIG_SPI_CS);
}

extern "C" void app_main(void)
{
        SPIMaster master(SPINum(1),
                MOSI_PIN,
                MISO_PIN,
                SCLK_PIN);

        shared_ptr<SPIDevice> spi_dev = master.create_dev(CS_PIN, Frequency::MHz(1));

        vector<uint8_t> write_data = {MPU9250_WHO_AM_I_REG_ADDR | READ_FLAG, 0x00};
        vector<uint8_t> result = spi_dev->transfer(write_data).get();

        printf("Result of WHO_AM_I register: 0x%02X\n", result[1]);

        this_thread::sleep_for(std::chrono::seconds(2));
}
