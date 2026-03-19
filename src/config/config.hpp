#ifndef BROKERINO_SRC_CONFIG_CONFIG_HPP
#define BROKERINO_SRC_CONFIG_CONFIG_HPP

#include <string>

namespace brokerino::config {

struct Config {
    int         port         = 1883;
    std::string db_path      = "brokerino.db";
    std::string log_level    = "info";
    bool        auth_enabled = true;
};

Config LoadConfig(const std::string& path);

} // namespace brokerino::config

#endif
