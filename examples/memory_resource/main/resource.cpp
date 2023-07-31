#include <utility>
#include "esp_memory_resource.hpp"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

constexpr auto TAG = "Memory Resource Example";
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(idf::memory::resource::tag, ESP_LOG_VERBOSE);
    auto memory_source = idf::memory::capabilities::dma;
    auto res = idf::memory::resource{memory_source};
    std::pmr::vector<std::byte> data{&res};
    data.reserve(2048);
    heap_caps_print_heap_info(idf::memory::get_caps(memory_source));
}
