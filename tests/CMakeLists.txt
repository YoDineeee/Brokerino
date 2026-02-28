include(GoogleTest)

# --- Unit tests ---
add_executable(brokerino_unit_tests
    unit/test_mqtt_codec.cpp
    unit/test_topic_trie.cpp
    unit/test_priority_queue.cpp
)

target_link_libraries(brokerino_unit_tests PRIVATE
    brokerino_core
    GTest::gtest_main
)

target_enable_warnings(brokerino_unit_tests)
gtest_discover_tests(brokerino_unit_tests)
