/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <thread>
#include <functional>

#include "gpio_cxx.hpp"
#include "gpio_intr_cxx.hpp"

using namespace idf;
using namespace std;
using namespace std::placeholders;

#define GPIO_NUM 10

class TriggerCounter {
public:
    TriggerCounter(uint32_t _gpio_num) : trigger_counter(0), gpio_num(_gpio_num) {}

    inline uint16_t get_interrupt_counter() { return trigger_counter; }

    inline void callback(uint32_t gpio_num) { trigger_counter++; }

    inline bool init() {
        cout << "initializing interrupt on GPIO " << gpio_num << endl;
        return GPIOIntr.gpio_intr_set(gpio_num, 0, 0, std::bind(&TriggerCounter::callback, this, _1));
    }
private:
    uint16_t trigger_counter;
    uint32_t gpio_num;
};

extern "C" void app_main(void)
{
    // initialize the GPIO that will trigger the interrupt on level high
    GPIOInput gpio_i(GPIONum(GPIO_NUM));
    gpio_i.set_pull_mode(GPIOPullMode::PULLDOWN());

    // create the trigger counter class
    TriggerCounter myTrigCounter(GPIO_NUM);
    myTrigCounter.init();

    uint16_t prev_counter = 0;
    while (true) {
        uint16_t counter = myTrigCounter.get_interrupt_counter();
        if (counter != prev_counter)
        {
            prev_counter = counter;
            printf("interrupt triggered %d\n", counter);
        }
        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
