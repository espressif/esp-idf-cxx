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
#define GPIO_NUM_C 27

/*****************************************************
 * Lambda function used as callback on GPIO A interrupt
 *****************************************************/
static size_t counter_a = 0;
static auto lambda_cb_gpio_a = [](GPIONum gpio_num) {
    if (gpio_num == GPIONum(GPIO_NUM_A))
    {
        counter_a++;
    }
};

static void task_intr_a(void* arg)
{
    // create interrupt on GPIO A using a lambda function as callback
    GPIOIntr gpio_intr_a{GPIONum(GPIO_NUM_A),
                         GPIOPullMode::PULLDOWN(),
                         GPIODriveStrength::STRONGEST(),
                         GPIOIntrType::POSEDGE(),
                         "lambda_cb_gpio_a",
                         lambda_cb_gpio_a};

    size_t cur_counter_a = 0;
    while(true)
    {
        if (counter_a != cur_counter_a)
        {
            printf("interrupt occured on GPIO A: %d\n", counter_a);
            cur_counter_a = counter_a;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/*****************************************************
 * Static function used a callback from a different
 * thread on GPIO B interrupt
 *****************************************************/
static size_t counter_b = 0;
static void static_cb_gpio_b(GPIONum gpio_num)
{
    if (gpio_num == GPIONum(GPIO_NUM_B))
    {
        counter_b++;
    }
}

static void task_intr_b(void* arg)
{
    GPIOIntr gpio_intr_b{GPIONum(GPIO_NUM_B),
                         GPIOPullMode::PULLDOWN(),
                         GPIODriveStrength::STRONGEST(),
                         GPIOIntrType::POSEDGE(),
                         "static_cb_gpio_b",
                         static_cb_gpio_b};

    size_t cur_counter_b = 0;
    while(true)
    {
        if (counter_b != cur_counter_b)
        {
            printf("interrupt occured on GPIO B: %d\n", counter_b);
            cur_counter_b = counter_b;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/*****************************************************
 * Member method used a callback from a different
 * thread on GPIO C interrupt
 *****************************************************/
class TestIntrC {
public:
    size_t first_counter_c;
    size_t second_counter_c;

    TestIntrC(): first_counter_c(0),
                      second_counter_c(0)
    {}

    void first_callback(GPIONum gpio_num)
    {
        if (gpio_num == GPIONum(GPIO_NUM_C))
        {
            first_counter_c++;
        }
    }

    void second_callback(GPIONum gpio_num)
    {
        if (gpio_num == GPIONum(GPIO_NUM_C))
        {
            second_counter_c++;
        }
    }
};

static void task_intr_c(void* arg)
{
    TestIntrC test_intr_c{};

    GPIOIntr gpio_intr_c{GPIONum(GPIO_NUM_C),
                         GPIOPullMode::PULLDOWN(),
                         GPIODriveStrength::STRONGEST(),
                         GPIOIntrType::POSEDGE(),
                         "first_callback",
                         std::bind(&TestIntrC::first_callback, &test_intr_c, _1)};

    gpio_intr_c.add_callback("second_callback",
                             std::bind(&TestIntrC::second_callback, &test_intr_c, _1));

    size_t cur_counter_c = 0;
    while(true)
    {
        if (test_intr_c.first_counter_c == test_intr_c.second_counter_c &&
            test_intr_c.first_counter_c != cur_counter_c)
        {
            printf("Both callbacks triggered on GPIO interrupt C: %d\n", test_intr_c.first_counter_c);
            cur_counter_c = test_intr_c.first_counter_c;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/*****************************************************
 * Main function
 *****************************************************/
extern "C" void app_main(void)
{
    // install ISR service before creating the interrupt
    try {
        start_service(GPIOIsrFlag().LEVEL1());
    }
    catch (const GPIOException& e) {
        printf("[0x%x]: %s\n", e.error, e.what());
    }

    // create interrupt on GPIO A in a different thread using lambda
    // function as callback
    xTaskCreatePinnedToCore(task_intr_a, "task_intr_a", 4096, NULL, 0, NULL, 0);

    // create interrupt on GPIO B in a different thread using static
    // function as callback
    xTaskCreatePinnedToCore(task_intr_b, "task_intr_b", 4096, NULL, 0, NULL, 1);

    // create interrupt on GPIO C in a different thread using member
    // method (non static) as callback. Two methods are registered on the
    // interrupt and are both triggered.
    xTaskCreatePinnedToCore(task_intr_c, "task_intr_c", 4096, NULL, 0, NULL, 1);
}