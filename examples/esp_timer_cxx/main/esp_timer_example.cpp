/* ESP Timer C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <thread>
#include <chrono>

#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"

using namespace std;
using namespace idf;
using namespace idf::esp_timer;

extern "C" void app_main(void)
{
    try {
        printf("Setting up timer to trigger in 500ms\n");
        ESPTimer timer([]() { printf("timeout\n"); });
        timer.start(chrono::microseconds(200 * 1000));

        this_thread::sleep_for(std::chrono::milliseconds(550));

        printf("Setting up timer to trigger periodically every 200ms\n");
        ESPTimer timer2([]() { printf("periodic timeout\n"); });
        timer2.start_periodic(chrono::microseconds(200 * 1000));

        this_thread::sleep_for(std::chrono::milliseconds(1050));
    } catch (const ESPException &e) {
        printf("Exception with error: %d\n", e.error);
    }
    printf("Finished\n");
}
