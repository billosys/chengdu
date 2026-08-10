cmake_minimum_required(VERSION 3.16)

project(pandapi_runtime VERSION 0.1.0 LANGUAGES CXX)

add_library(pandapi_runtime STATIC
  src/runtime.cpp
  src/status.cpp
  src/status_io.cpp
)
add_library(pandapi::runtime ALIAS pandapi_runtime)

target_include_directories(pandapi_runtime
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(pandapi_runtime PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

if(MSVC)
  target_compile_options(pandapi_runtime PRIVATE /W4)
else()
  target_compile_options(pandapi_runtime PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_executable(pandapi_runtime_smoke
  tests/runtime_smoke.cpp
)
target_link_libraries(pandapi_runtime_smoke PRIVATE pandapi::runtime)

set_target_properties(pandapi_runtime_smoke PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(NAME pandapi_runtime_smoke COMMAND pandapi_runtime_smoke)

add_executable(pandapi_status_result_smoke
  tests/status_result_smoke.cpp
)
target_link_libraries(pandapi_status_result_smoke PRIVATE pandapi::runtime)

set_target_properties(pandapi_status_result_smoke PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(NAME pandapi_status_result_smoke COMMAND pandapi_status_result_smoke)

add_executable(pandapi_status_io_smoke
  tests/status_io_smoke.cpp
)
target_link_libraries(pandapi_status_io_smoke PRIVATE pandapi::runtime)

set_target_properties(pandapi_status_io_smoke PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(NAME pandapi_status_io_smoke COMMAND pandapi_status_io_smoke)
