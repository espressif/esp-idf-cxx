#pragma once

#include <memory_resource>
#include <utility>
#include <type_traits>
#include "esp_heap_caps.h"

namespace idf::memory {

/**
 * Memory capabilities for memory allocation
 *
 * Refer to IDF documentation on memory allocation for the usage of each of them.
 */
enum class capabilities : uint32_t {
    not_specified = MALLOC_CAP_DEFAULT,
    adressable_8bit = MALLOC_CAP_8BIT,
    adressable_32bit = MALLOC_CAP_32BIT,
    dma = MALLOC_CAP_DMA,
    executable = MALLOC_CAP_EXEC,
    from_spiram = MALLOC_CAP_SPIRAM,
    internal = MALLOC_CAP_INTERNAL,
    retention_dma = MALLOC_CAP_RETENTION,
    rtc = MALLOC_CAP_RTCRAM
};

constexpr capabilities operator|(const capabilities lhs, const capabilities rhs)
{
    return capabilities{static_cast<std::underlying_type_t<capabilities>>(lhs) | static_cast<std::underlying_type_t<capabilities>>(rhs)};
}

constexpr auto get_caps(const capabilities cap) {
    return static_cast<std::underlying_type_t<capabilities>>(cap);
}

/**
* @brief Memory resource to be used with polymorphic allocators
*/
class resource : public std::pmr::memory_resource {
public:
    static constexpr auto tag = "esp_memory_resource";
    /**
    * Construct a resource with the selected capabilities and upstream resource.
    *
    * @param cap capabilities to use in this allocator. Refer to idf memory allocation documentation to understand the effects. Default: capabilities::not_specified
    * @param upstream memory resource to use when allocation fails. It can be used to compose memory resources. Default: default resource.
    */
    explicit resource(capabilities cap=capabilities::not_specified, std::pmr::memory_resource * upstream = std::pmr::get_default_resource()) : cap(cap), upstream(upstream) {}

private:
    void * do_allocate(size_t bytes, [[maybe_unused]] size_t alignment) override;
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override;
    [[nodiscard]] bool do_is_equal(const memory_resource &other) const noexcept override;
    capabilities cap;
    std::pmr::memory_resource * upstream;
};


}
