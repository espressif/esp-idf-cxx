#include "esp_memory_resource.hpp"
#include "esp_log.h"
#include <utility>

namespace idf::memory {


void * resource::do_allocate(size_t bytes, size_t alignment)
{

    if(auto *mem = heap_caps_aligned_alloc(alignment, bytes, static_cast<std::underlying_type_t<capabilities>>(cap)); mem != nullptr) {
        ESP_LOGD(tag, "Allocated %d bytes at %p", bytes, mem);
        return mem;
    }
        ESP_LOGD(tag, "Failed to get memory from resource, allocating from upstream");
    return upstream->allocate(bytes, alignment);

}
void resource::do_deallocate(void *ptr, [[maybe_unused]] size_t bytes, [[maybe_unused]]  size_t alignment)  {
    free(ptr);
    ESP_LOGD(tag, "Freed");
}

[[nodiscard]] bool resource::do_is_equal(const memory_resource &other) const noexcept
{
    return this == &other;
}

}
