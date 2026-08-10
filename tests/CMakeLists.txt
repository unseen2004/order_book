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
