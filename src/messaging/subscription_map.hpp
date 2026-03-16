#ifndef BROKERINO_SRC_MESSAGING_SUBSCRIPTION_MAP_HPP_
#define BROKERINO_SRC_MESSAGING_SUBSCRIPTION_MAP_HPP_

#include "messaging/topic_trie.hpp"
#include "messaging/message.cpp"
#include <string>
#include<unordered_map>
#include <vector>

namespace brokerino::messaging{

    class SubscriptionMap{
        public:
        //client subscribe to a topic filter
        void Subscribe(const std::string& client_id , const std::string& topic_filter , std::uint8_t qos);

        //Remove a single subscription 
        void Unsubscribe(const std::string& client_id , const std::string& topic_filter);

        // removing the client from the subscription of the client 
        void RemoveClient(*client_id , *topic_filter);

        //Remove all client_ids whose filters match the given topics 
        std::vector<std::string>Match(const std::string& topic ) const ;

        private:

        struct ClientEntry{
            TopicTrie trie;
            std::unordered_map<std::string,std::uint8_t> filters; //filter granted --> Qos

        };

        std::unordered_map<std::string,ClientEntry> clients_; //client_id ->entry 
    };
} // namespace brokerino::messaging

#endif