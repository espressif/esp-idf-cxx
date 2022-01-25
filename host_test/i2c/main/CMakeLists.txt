idf_component_get_property(cpp_component esp-idf-cxx COMPONENT_DIR)

idf_component_register(SRCS "i2c_cxx_test.cpp"
                    INCLUDE_DIRS
                    "."
                    "${cpp_component}/host_test/fixtures"
                    "${cpp_component}/private_include"
                    $ENV{IDF_PATH}/tools/catch
                    PRIV_REQUIRES driver cmock)

target_link_libraries(${COMPONENT_LIB} -lpthread)
