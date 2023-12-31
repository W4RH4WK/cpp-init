file(GLOB_RECURSE example_srcs CONFIGURE_DEPENDS *.cpp *.hpp *.inc)

# Filter Tests
set(example_tests_srcs ${example_srcs})
list(FILTER example_tests_srcs INCLUDE REGEX "\\.test\\.(cpp|hpp|inc)")
list(FILTER example_srcs EXCLUDE REGEX "\\.test\\.(cpp|hpp|inc)")

add_library(example STATIC ${example_srcs})
example_compile_options(example)
target_include_directories(example PUBLIC ${PROJECT_SOURCE_DIR}/code)
target_precompile_headers(example PUBLIC example_pch.hpp)
target_link_libraries(example PUBLIC fmt)

add_executable(example_tests ${example_tests_srcs})
example_compile_options(example_tests)
target_link_libraries(example_tests PRIVATE example Catch2::Catch2WithMain)

include(CTest)
catch_discover_tests(example_tests)
