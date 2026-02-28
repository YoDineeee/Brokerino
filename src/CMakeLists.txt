add_library(brokerino_core STATIC
    # Config
    config/config.cpp

    # Codec / protocol
    core/mqtt_codec.cpp

    # Message routing
    message/topic_trie.cpp
    message/subscription_map.cpp

    # Persistence
    persistence/storage.cpp

    # Features
    features/priority_queue.cpp
    features/enricher.cpp

    # Auth
    auth/simple_auth.cpp

    # Networking
    networking/connection.cpp

    # Broker
    broker/broker.cpp
)

target_include_directories(brokerino_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(brokerino_core PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
    asio::asio
    Threads::Threads
)

if(ENABLE_WEBHOOKS)
    target_link_libraries(brokerino_core PUBLIC cpr::cpr)
    target_compile_definitions(brokerino_core PUBLIC BROKERINO_WEBHOOKS)
endif()

target_enable_warnings(brokerino_core)

# --- Main executable ---
add_executable(brokerino main.cpp)
target_link_libraries(brokerino PRIVATE brokerino_core)
target_enable_warnings(brokerino)