#ifndef BROKERINO_SRC_MESSAGING_SUBSCRIPTION_MAP_HPP
#define BROKERINO_SRC_MESSAGING_SUBSCRIPTION_MAP_HPP

#include "messaging/topic_trie.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace brokerino::messaging {

class SubscriptionMap {
public:
    void Subscribe  (const std::string& client_id,
                     const std::string& topic_filter,
                     std::uint8_t qos);

    void Unsubscribe(const std::string& client_id,
                     const std::string& topic_filter);

    void RemoveClient(const std::string& client_id);

    std::vector<std::string> Match(const std::string& topic) const;

private:
    struct ClientEntry {
        TopicTrie trie;
        std::unordered_map<std::string, std::uint8_t> filters;
    };

    std::unordered_map<std::string, ClientEntry> clients_;
};

} // namespace brokerino::messaging

#endif
