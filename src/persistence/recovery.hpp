#ifndef BROKERINO_SRC_PERSISTENCE_RECOVERY_HPP_
#define BROKERINO_SRC_PERSISTENCE_RECOVERY_HPP_

#include "persistence/storage.hpp"
#include "session/mqtt_session.hpp"
#include "messaging/subscription_map.hpp"
#include <vector>

namespace brokerino::persistence{
    void Recover(Storage& storage, std::vector<core::Session>&  session_out,  messaging::SubscriptionMap& sub_map_out );
}

#endif
