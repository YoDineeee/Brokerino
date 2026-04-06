/**
 * @file topic_trie.hpp
 * @brief Efficient MQTT topic matching using a Trie (prefix tree).
 *
 * Supports MQTT wildcards `+` (single level) and `#` (multi-level).
 * Used internally by SubscriptionMap for fast topic matching.
 */


#ifndef BROKERINO_SRC_MESSAGING_TOPIC_TRIE_HPP
#define BROKERINO_SRC_MESSAGING_TOPIC_TRIE_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace brokerino::messaging {

struct TrieNode {
    std::unordered_map<std::string, TrieNode> children;
    bool is_end = false;
};


/**
 * @class TopicTrie
 * @brief Trie-based structure for MQTT topic filter matching.
 */

class TopicTrie {
public:
    void Insert(const std::string& topic_filter);
    void Remove(const std::string& topic_filter);
    bool Match (const std::string& topic) const;

private:
    static std::vector<std::string> Split(const std::string& topic);

    bool MatchNode(const TrieNode& node,
                   const std::vector<std::string>& levels,
                   std::size_t index) const;

    TrieNode root_;
};

} // namespace brokerino::messaging

#endif
