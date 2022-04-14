idf_component_get_property(cpp_component esp-idf-cxx COMPONENT_DIR)

idf_component_register(SRCS "system_cxx_test.cpp"
                    "${cpp_component}/esp_exception.cpp"
                    INCLUDE_DIRS
                    "."
                    "${cpp_component}/include"
                    $ENV{IDF_PATH}/tools/catch
                    PRIV_REQUIRES driver)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} --coverage)
