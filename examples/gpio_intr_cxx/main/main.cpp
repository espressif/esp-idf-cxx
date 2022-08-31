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
    TriggerCounter(GPIONum _gpio_num) : trigger_counter(0), gpio_num(_gpio_num) {}

    inline uint16_t get_interrupt_counter() { return trigger_counter; }

    inline void callback(GPIONum gpio_num) { trigger_counter++; }

    inline void init() {
        cout << "initializing interrupt on GPIO " << gpio_num.get_num() << endl;
        try {
            GPIOIntr.gpio_start_isr_service(GPIOIsrFlag().LEVEL1());
            GPIOIntr.gpio_set_intr(GPIONum(GPIO_NUM), GPIOIntrType::POSEDGE(), std::bind(&TriggerCounter::callback, this, _1));
        }
        catch (const GPIOException& e)
        {
            printf("Error 0x%x occured\n", e.error);
        }
    }
private:
    uint16_t trigger_counter;
    GPIONum gpio_num;
};

extern "C" void app_main(void)
{
    // initialize the GPIO that will trigger the interrupt on level high
    GPIOInput gpio_i(GPIONum(GPIO_NUM));
    gpio_i.set_pull_mode(GPIOPullMode::PULLDOWN());

    // create the trigger counter class
    TriggerCounter myTrigCounter(GPIONum(GPIO_NUM));
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
