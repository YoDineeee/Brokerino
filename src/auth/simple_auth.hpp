/**
 * @file simple_auth.hpp
 * @brief Simple authentication module (username/password + token).
 *
 * Used during CONNECT handling to decide whether to accept a client.
 */
#ifndef BROKERINO_SRC_AUTH_SIMPLE_AUTH_HPP_
#define BROKERINO_SRC_AUTH_SIMPLE_AUTH_HPP_

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace brokerino::auth {

/**
 * @class SimpleAuth
 * @brief Lightweight authentication provider.
 *
 * Supports two methods:
 *   - Traditional username + password
 *   - Bearer token (for the Telegram bot, for example)
 */
class SimpleAuth {
 public:
  void AddUser(const std::string& username, const std::string& password);
  void AddToken(const std::string& token);

  bool AuthenticateUser(const std::string& username,
                        const std::string& password) const;

  bool AuthenticateToken(const std::string& token) const;

  /**
   * @brief Main authentication function used by Broker.
   * @return true if either token or username/password is valid.
   */
  bool Authenticate(const std::string& username,
                    const std::string& password,
                    const std::string& token) const;

 private:
  std::unordered_map<std::string, std::string> users_;
  std::unordered_set<std::string>              tokens_;
};

}  // namespace brokerino::auth

#endif  // BROKERINO_SRC_AUTH_SIMPLE_AUTH_HPP_