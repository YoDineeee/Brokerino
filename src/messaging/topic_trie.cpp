#include "messaging/topic_trie.hpp"

namespace brokerino::messaging{
    std::vector<std::string> TopicTrie::Split(const std::string& topic){
        std::vector<std::string> levels;
        std::string current ;

        for (const char c : topic){
            if ( c == '/'){
                levels.push_back(current);
                current.clear();
            } else {
                current += c ;

            }
        }
          levels.push_back(current);
         return levels ;
    }
  
    void TopicTrie::Insert(const std::string& topic_filter){
        TrieNode* node = &root_ ;
        
        for (const auto& level : Split (topic_filter)){
            node = &node -> children[level];
        }
        node -> is_end = true ;
    }


    void TopicTrie ::


} // namespace brokerino::messaging