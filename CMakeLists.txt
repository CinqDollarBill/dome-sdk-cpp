cmake_minimum_required(VERSION 3.14)
project(dome_sdk_cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)

option(USE_SYSTEM_JSON "Use system-installed nlohmann_json" OFF)

include(FetchContent)

if(USE_SYSTEM_JSON)
    find_package(nlohmann_json REQUIRED)
    set(JSON_TARGET nlohmann_json::nlohmann_json)
else()
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(json)
    set(JSON_TARGET nlohmann_json::nlohmann_json)
endif()

add_library(dome_sdk
    src/http_client.cpp
    src/base_endpoint.cpp
    src/endpoints/market_endpoints.cpp
    src/endpoints/orders_endpoints.cpp
    src/endpoints/wallet_endpoints.cpp
    src/endpoints/activity_endpoints.cpp
    src/endpoints/polymarket_websocket.cpp
    src/endpoints/polymarket_client.cpp
    src/client.cpp
)

target_include_directories(dome_sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dome_sdk
    PUBLIC
        CURL::libcurl
        ${JSON_TARGET}
)

add_executable(polymarket_demo examples/polymarket_demo.cpp)
target_link_libraries(polymarket_demo PRIVATE dome_sdk)

add_executable(main examples/main.cpp)
target_link_libraries(main PRIVATE dome_sdk)

install(TARGETS dome_sdk
    EXPORT dome_sdk_targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
