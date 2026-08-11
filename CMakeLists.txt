cmake_minimum_required(VERSION 3.10)
project(hft)

set(CMAKE_CXX_STANDARD 17)

add_executable(hft
        spsc.h
        main.cc
        LogEvent.h
        mmap_logger.h
        order_pool.h
        price_level.h
        order_map.h
        itch_parser.h
        fix_encoder.h)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
