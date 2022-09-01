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

#define GPIO_NUM_A 10
#define GPIO_NUM_B 11

class TriggerCounter {
public:
    TriggerCounter(GPIONum _gpio_num) : trigger_counter(0), gpio_num(_gpio_num) {
        printf("trigger count initialized\n");
    }

    inline uint16_t get_interrupt_counter() { return trigger_counter; }

    inline void callback(GPIONum gpio_num) { trigger_counter++; }

    inline void init(std::string _cb_name) {
        cout << "initializing interrupt on GPIO " << gpio_num.get_num() << endl;
        cb_name = _cb_name;
        try {
            GPIOIntr.start_isr_service(GPIOIsrFlag().LEVEL1().LEVEL2());
            GPIOIntr.set_type(gpio_num, GPIOIntrType::POSEDGE());
            GPIOIntr.add_callback(gpio_num, cb_name, std::bind(&TriggerCounter::callback, this, _1));
        }
        catch (const GPIOException& e) {
            printf("Error 0x%x occured\n", e.error);
        }
    }

    inline void remove_interrupt() {
        try {
            GPIOIntr.remove_callback(gpio_num, cb_name);
        }
        catch (const GPIOException& e) {
            printf("Error 0x%x occured\n", e.error);
        }
    }
private:
    uint16_t trigger_counter;
    std::string cb_name;
    GPIONum gpio_num;
};

extern "C" void app_main(void)
{
    GPIONum gpio_num_a(GPIO_NUM_A);
    GPIONum gpio_num_b(GPIO_NUM_B);

    // initialize the GPIOs that will trigger the interrupts on level high
    GPIOInput gpio_i_a(gpio_num_a);
    gpio_i_a.set_pull_mode(GPIOPullMode::PULLDOWN());

    GPIOInput gpio_i_b(gpio_num_b);
    gpio_i_b.set_pull_mode(GPIOPullMode::PULLDOWN());

    // create the trigger counter class
    TriggerCounter trig_counter_a(gpio_num_a);
    trig_counter_a.init("cb_name_a");

    TriggerCounter trig_counter_b(gpio_num_b);
    trig_counter_b.init("cb_name_b");

    uint16_t prev_counter_a = 0;
    uint16_t prev_counter_b = 0;
    while (true) {
        uint16_t counter_a = trig_counter_a.get_interrupt_counter();
        uint16_t counter_b = trig_counter_b.get_interrupt_counter();
        if (counter_a != prev_counter_a || counter_b != prev_counter_b)
        {
            prev_counter_a = counter_a;
            prev_counter_b = counter_b;
            printf("GPIO 10: %d | GPIO 11: %d\n", counter_a, counter_b);
        }
        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
