cmake_minimum_required(VERSION 3.20)

project(mipmap_plugin LANGUAGES CXX)

option(MIPMAP_USE_CXX20 "Build with C++20 instead of C++17" ON)
set(CMAKE_CXX_STANDARD 17)
if(MIPMAP_USE_CXX20)
  set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "MipMap plugin requires Clang 15+.")
endif()
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15)
  message(FATAL_ERROR "MipMap plugin requires Clang 15+.")
endif()

set(ENDSTONE_PLUGIN_DIR "" CACHE PATH "Endstone plugin load directory")

include(FetchContent)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.14.0
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(mipmap_plugin SHARED
  native/src/mipmap_plugin.cpp
)

if(ENDSTONE_PLUGIN_DIR)
  set_target_properties(mipmap_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${ENDSTONE_PLUGIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${ENDSTONE_PLUGIN_DIR}"
  )
endif()

target_compile_options(mipmap_plugin PRIVATE -stdlib=libc++)
target_link_options(mipmap_plugin PRIVATE -stdlib=libc++)

target_link_libraries(mipmap_plugin PRIVATE
  httplib::httplib
  nlohmann_json::nlohmann_json
)
