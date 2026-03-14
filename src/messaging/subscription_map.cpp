#include "message/subscription_map.hpp"

namespace brokerino::message{
    void SubscriptionMap :: Subscribe(const std::string& client_id , const std::strings& topic_filter , std::uint8_t qos  ){
        auto &entry = clients_[client_id];
        entry.trie.Insert(topic_filter);
        entry.filters[topic_filter] = qos ;

    }

    void SubscriptionMap :: Unsubscribe (const std::string& client_id , const std::string& topic_filter){

        //iterator 
        auto it = clients_.find(client_id);
        if ( it ==clients_.end())return;

        it ->second.trie.Remove(topic_filter);
        it -> second.filters.erase(topic_filter);
    }

    void SubscriptionMap :: RemoveClient (const std::string& client_id){
        clients_.erase(client_id);
    }

    std::vector<std::string>SubscriptionMap::Match(const std::string& topic) const {
        std::vector<std::string> matched;

        for (const auto& [client_id, entry ] : clients_){
            if (entry.trie.Match(topic)){
                matched.push_back(client_id);
            }
        }

        return matched ;
    }


    
}