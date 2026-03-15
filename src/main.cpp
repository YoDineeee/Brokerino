#include "broker/broker.hpp"
#include "config/config.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    const std::string config_path = (argc > 1) ? argv[1] : "brokerino.conf";
    const auto cfg = brokerino::config::LoadConfig(config_path);

    std::cout << "[BROKERINO] Starting on port " << cfg.port << "\n";
    std::cout << "[BROKERINO] Database: "        << cfg.db_path << "\n";

    try {
        brokerino::Broker broker(cfg.db_path, cfg.port);
        broker.Run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }

    return 0;
}