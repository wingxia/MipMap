cmake_minimum_required(VERSION 3.20)

project(endstone_mipmap_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  httplib
  URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.14.3.tar.gz
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(endstone_mipmap_cpp SHARED
  src/DataStore.cpp
  src/HttpServer.cpp
  src/MapPlugin.cpp
)

target_include_directories(endstone_mipmap_cpp
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(endstone_mipmap_cpp
  PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(endstone_mipmap_cpp PRIVATE -stdlib=libc++)
  target_link_options(endstone_mipmap_cpp PRIVATE -stdlib=libc++)
endif()
