idf_component_register(SRCS "gpio_cxx_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "../../fixtures"
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES driver cmock esp-idf-cxx)
