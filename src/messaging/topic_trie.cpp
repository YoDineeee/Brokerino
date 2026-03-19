#include "messaging/topic_trie.hpp"

namespace brokerino::messaging {

std::vector<std::string> TopicTrie::Split(const std::string& topic) {
    std::vector<std::string> levels;
    std::string current;

    for (const char c : topic) {
        if (c == '/') {
            levels.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    levels.push_back(current);
    return levels;
}

void TopicTrie::Insert(const std::string& topic_filter) {
    TrieNode* node = &root_;

    for (const auto& level : Split(topic_filter))
        node = &node->children[level];

    node->is_end = true;
}

void TopicTrie::Remove(const std::string& topic_filter) {
    TrieNode* node = &root_;

    for (const auto& level : Split(topic_filter)) {
        auto it = node->children.find(level);
        if (it == node->children.end()) return;
        node = &it->second;
    }

    node->is_end = false;
}

bool TopicTrie::Match(const std::string& topic) const {
    const auto levels = Split(topic);
    return MatchNode(root_, levels, 0);
}

bool TopicTrie::MatchNode(const TrieNode& node,
                           const std::vector<std::string>& levels,
                           std::size_t index) const {
    // '#' matches everything remaining
    auto hash_it = node.children.find("#");
    if (hash_it != node.children.end()) return true;

    // consumed all levels
    if (index == levels.size()) return node.is_end;

    const std::string& current = levels[index];

    // '+' matches any single level
    auto plus_it = node.children.find("+");
    if (plus_it != node.children.end())
        if (MatchNode(plus_it->second, levels, index + 1)) return true;

    // exact match
    auto exact_it = node.children.find(current);
    if (exact_it != node.children.end())
        if (MatchNode(exact_it->second, levels, index + 1)) return true;

    return false;
}

} // namespace brokerino::messaging
