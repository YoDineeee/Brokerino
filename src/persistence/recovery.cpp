#include "persistence/recovery.hpp"

namespace brokerino::persistence {

void Recover(Storage&                    storage,
             std::vector<core::Session>& sessions_out,
             messaging::SubscriptionMap& sub_map_out) {

    sessions_out = storage.LoadAllSessions();

    for (const auto& session : sessions_out) {
        if (session.clean_session) continue;

        for (const auto& [filter, qos] : session.subscriptions)
            sub_map_out.Subscribe(session.client_id, filter, qos);
    }
}

} // namespace brokerino::persistence