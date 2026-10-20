cmake_minimum_required(VERSION 3.20)
project(github_crawler VERSION 0.1.0 LANGUAGES CXX)


# Ubuntu 22.04 LTS has a broken cpp-httplib package, so we need to manually find it if the config file is not found.
find_package(httplib CONFIG QUIET)
if(NOT TARGET httplib::httplib)
    find_path(HTTPLIB_INCLUDE_DIR NAMES httplib.h REQUIRED)
    find_library(HTTPLIB_LIBRARY NAMES cpp-httplib REQUIRED)

    add_library(httplib::httplib UNKNOWN IMPORTED)
    set_target_properties(httplib::httplib PROPERTIES
        IMPORTED_LOCATION "${HTTPLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${HTTPLIB_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS
            "CPPHTTPLIB_OPENSSL_SUPPORT;CPPHTTPLIB_ZLIB_SUPPORT;CPPHTTPLIB_BROTLI_SUPPORT"
    )
endif()

find_package(nlohmann_json CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(github_crawler 
            src/main.cpp
            src/github_client.cpp
            src/repository_parser.cpp
            src/repository_store.cpp
            src/data_loader.cpp
            src/analyzer.cpp
            src/reporter.cpp
            src/exporter.cpp
            src/repository.hpp
            src/github_client.hpp
            src/repository_parser.hpp
            src/repository_store.hpp
            src/data_loader.hpp
            src/analyzer.hpp
            src/reporter.hpp
            src/exporter.hpp)

target_include_directories(github_crawler PRIVATE src)

target_compile_features(github_crawler PRIVATE cxx_std_20)
target_compile_options(github_crawler PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(github_crawler PRIVATE
                    httplib::httplib
                    nlohmann_json::nlohmann_json
)

include(CTest)
enable_testing()

add_executable(test_data_loader
    tests/test_data_loader.cpp
    src/data_loader.cpp
    src/repository.hpp
    src/data_loader.hpp
)
target_include_directories(test_data_loader PRIVATE src)
target_compile_features(test_data_loader PRIVATE cxx_std_20)
target_compile_options(test_data_loader PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_data_loader PRIVATE
    nlohmann_json::nlohmann_json
)
add_test(NAME test_data_loader COMMAND test_data_loader)

add_executable(test_analyzer
    tests/test_analyzer.cpp
    src/analyzer.cpp
    src/repository.hpp
    src/analyzer.hpp
)
target_include_directories(test_analyzer PRIVATE src)
target_compile_features(test_analyzer PRIVATE cxx_std_20)
target_compile_options(test_analyzer PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME test_analyzer COMMAND test_analyzer)

add_executable(test_exporter
    tests/test_exporter.cpp
    src/exporter.cpp
    src/repository.hpp
    src/exporter.hpp
)
target_include_directories(test_exporter PRIVATE src)
target_compile_features(test_exporter PRIVATE cxx_std_20)
target_compile_options(test_exporter PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_exporter PRIVATE
    nlohmann_json::nlohmann_json
)
add_test(NAME test_exporter COMMAND test_exporter)
