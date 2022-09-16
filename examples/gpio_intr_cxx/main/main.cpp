/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <thread>
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "gpio_cxx.hpp"
#include "gpio_intr_cxx.hpp"

using namespace idf;
using namespace std;
using namespace std::placeholders;

#define GPIO_NUM_A 12
#define GPIO_NUM_B 13

class IntHdlr {
public:
    IntHdlr(const GPIONum gpio_num, const GPIOPullMode mode, const GPIODriveStrength strength, const GPIOIntrType type):
        gpio_intr(gpio_num, mode, strength, type, "name", std::bind(&IntHdlr::callback, this, _1)),
        counter(0),
        missed_counter(0)
    {
    }

    size_t get_counter() const 
    {
        return counter;
    }

    size_t get_missed() const
    {
        return missed_counter;
    }

    void callback(GPIONum gpio_num)
    {
        if (gpio_intr.get_gpio_num() == gpio_num)
        {
            counter++;
        }
        else{
            missed_counter++;
        }
    }
private:
    GPIOIntr gpio_intr;
    size_t counter;
    size_t missed_counter;
};

extern "C" void app_main(void)
{
    // install ISR service before any interrupt related code
    try {
        start_service(GPIOIsrFlag().LEVEL1());
    }
    catch (const GPIOException& e) {
        printf("[0x%x]: %s\n", e.error, e.what());
    }

    IntHdlr hdlr{GPIONum(GPIO_NUM_A), GPIOPullMode::PULLDOWN(), GPIODriveStrength::STRONGEST(), GPIOIntrType::POSEDGE()};

    size_t cur_cnt = 0;
    size_t cur_missed = 0;
    while(1) {
        size_t cnt = hdlr.get_counter();
        if (cur_cnt != cnt)
        {
            cur_cnt = cnt;
            printf("interrupt occurred %d\n", cnt);
        }

        size_t missed = hdlr.get_missed();
        if (cur_missed != missed)
        {
            cur_missed = missed;
            printf("missed interrupt occurred %d\n", missed);
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
