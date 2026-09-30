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
            src/repository_store.cpp)

target_compile_features(github_crawler PRIVATE cxx_std_20)
target_link_libraries(github_crawler PRIVATE
                    httplib::httplib
                    nlohmann_json::nlohmann_json
)

include(CTest)
enable_testing()

