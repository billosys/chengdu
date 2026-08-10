cmake_minimum_required(VERSION 3.16)

project(pandapi_runtime VERSION 0.1.0 LANGUAGES CXX)

add_library(pandapi_runtime STATIC
  src/cli_policy.cpp
  src/fixture.cpp
  src/normalization.cpp
  src/process_fixture.cpp
  src/provenance.cpp
  src/runtime.cpp
  src/status.cpp
  src/status_io.cpp
  src/tty.cpp
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

add_executable(pandapi_cli_tty_provenance_smoke
  tests/cli_tty_provenance_smoke.cpp
)
target_link_libraries(pandapi_cli_tty_provenance_smoke PRIVATE pandapi::runtime)

set_target_properties(pandapi_cli_tty_provenance_smoke PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(
  NAME pandapi_cli_tty_provenance_smoke
  COMMAND pandapi_cli_tty_provenance_smoke)

add_executable(pandapi_fixture_substrate_smoke
  tests/fixture_substrate_smoke.cpp
)
target_link_libraries(pandapi_fixture_substrate_smoke PRIVATE pandapi::runtime)

set_target_properties(pandapi_fixture_substrate_smoke PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

add_test(
  NAME pandapi_fixture_substrate_smoke
  COMMAND pandapi_fixture_substrate_smoke)

find_package(Catch2 QUIET CONFIG)
if(Catch2_FOUND)
  add_executable(pandapi_fixture_catch2_smoke
    tests/fixture_catch2_smoke.cpp
  )
  target_link_libraries(
    pandapi_fixture_catch2_smoke
    PRIVATE
      pandapi::runtime
      Catch2::Catch2WithMain
  )

  set_target_properties(pandapi_fixture_catch2_smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
  )

  add_test(
    NAME pandapi_fixture_catch2_smoke
    COMMAND pandapi_fixture_catch2_smoke)
else()
  message(STATUS "Catch2 not found; optional test-only fixture seam gate skipped")
endif()
