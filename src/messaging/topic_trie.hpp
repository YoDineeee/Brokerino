#ifndef BROKERINO_SRC_MESSAGING_TOPIC_TRIE_HPP_
#define BROKERINO_SRC_MESSAGING_TOPIC_TRIE_HPP_

#include <string>
#include <vector>
#include<unordered_map>

namespace brokerino::messaging{
    struct TrieNode{
        std::unordered_map<std::string,TrieNode> children;
        bool is_end = false ; // the subscription terminates

    };

    class TopicTrie{
        public:
        //Add a topic filter  example "home/+/peace" or "home/#"
        void  Insert(const std::strings& topic_filter);
        //Remove a topic filter
        void Remove(const std::string& topic_filter);
        //Check if a concrete topic matches any stored filter
        bool Match(const std::string& topic) const;

        private:
        //split "home/sensors/temp" -> ["home","sensors","temp"]
        static std::vector<std::string>Split(const std::string& topic);

        //Recrusive much helper 
        bool MatchNode(const TrieNode& node , const std::vector<std::string>& levels, std::size_t index ) const;

        TrieNode root_;

    };

    
} // namespace brokerino::messaging

#endif