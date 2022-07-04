/*
 * SPDX-FileCopyrightText: 2020 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cpp_exceptions

#include "esp_exception.hpp"

namespace idf {

ESPException::ESPException(esp_err_t error) : error(error) { }

const char *ESPException::what() const noexcept {
    return esp_err_to_name(error);
}

} // namespace idf

#endif // __cpp_exceptions
