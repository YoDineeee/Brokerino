#include "config/config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace brokerino::config {

Config LoadConfig(const std::string& path) {
    Config cfg;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "[Config] No config file found at "
                  << path << " -- using defaults\n";
        return cfg;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string key, value;
        if (!std::getline(ss, key, '=')) continue;
        if (!std::getline(ss, value))    continue;

        if      (key == "port")         cfg.port         = std::stoi(value);
        else if (key == "db_path")      cfg.db_path      = value;
        else if (key == "log_level")    cfg.log_level    = value;
        else if (key == "auth_enabled") cfg.auth_enabled = (value == "true");
    }

    return cfg;
}

} // namespace brokerino::config
