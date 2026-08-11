add_executable(order_pool_test order_pool_test.cc)
target_link_libraries(order_pool_test gtest_main)
target_include_directories(order_pool_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME order_pool_test COMMAND order_pool_test)

add_executable(spsc_test spsc_test.cc)
target_link_libraries(spsc_test gtest_main)
target_include_directories(spsc_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME spsc_test COMMAND spsc_test)

add_executable(property_test property_test.cc)
target_link_libraries(property_test gtest_main)
target_include_directories(property_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME property_test COMMAND property_test)

add_executable(stress_test stress_test.cc)
target_link_libraries(stress_test gtest_main)
target_include_directories(stress_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME stress_test COMMAND stress_test)

add_executable(logger_test logger_test.cc)
target_link_libraries(logger_test gtest_main)
target_include_directories(logger_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME logger_test COMMAND logger_test)

add_executable(price_level_test price_level_test.cc)
target_link_libraries(price_level_test gtest_main)
target_include_directories(price_level_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME price_level_test COMMAND price_level_test)

add_executable(order_map_test order_map_test.cc)
target_link_libraries(order_map_test gtest_main)
target_include_directories(order_map_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME order_map_test COMMAND order_map_test)

add_executable(itch_parser_test itch_parser_test.cc)
target_link_libraries(itch_parser_test gtest_main)
target_include_directories(itch_parser_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME itch_parser_test COMMAND itch_parser_test)

add_executable(fix_encoder_test fix_encoder_test.cc)
target_link_libraries(fix_encoder_test gtest_main)
target_include_directories(fix_encoder_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME fix_encoder_test COMMAND fix_encoder_test)

add_executable(limit_order_book_test limit_order_book_test.cc)
target_link_libraries(limit_order_book_test gtest_main)
target_include_directories(limit_order_book_test PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME limit_order_book_test COMMAND limit_order_book_test)
