#include "persistence/ storage.hpp"
#include "core /mqtt_session.hpp"
#include "message /subscription_map.hpp"
#include  <vector>

namespace brokerino:: persistence {
    //called once on broker
    void Recover(Storage& storage, std::vector<core::Session>& session_out, message::subscriptionMap& sub_map_out ) {
        session_out = storage.LoadALLSessions();

        for (const auto& session:sessions_out){
            if (session.clean_session) continue;

            for (const auto& [filter,qos]:session.subscriptions){
                sub_map_out.Subscribe(session.client_id,filter,qos);
            }
        }
    }

} // namespace brokerino::persistence
