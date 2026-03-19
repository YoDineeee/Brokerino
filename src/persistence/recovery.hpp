#ifndef BROKERINO_SRC_PERSISTENCE_RECOVERY_HPP
#define BROKERINO_SRC_PERSISTENCE_RECOVERY_HPP

#include "persistence/storage.hpp"
#include "session/mqtt_session.hpp"
#include "messaging/subscription_map.hpp"

#include <vector>

namespace brokerino::persistence {

// ============================================================
// Session Recovery Interface
// ============================================================

// Restores broker state from persistent storage.
//
// This function is responsible for:
// - Loading all stored MQTT sessions
// - Rebuilding the in-memory subscription map
//
// Notes:
// - Sessions marked as `clean_session = true` are ignored during recovery
//   (per MQTT 3.1.1 specification).
// - Only persistent sessions contribute to the restored subscription map.
//
// Parameters:
// - storage        : abstraction over persistent storage (file/DB/etc.)
// - sessions_out   : output vector containing all restored sessions
// - sub_map_out    : subscription map rebuilt from stored sessions
void Recover(Storage& storage,
             std::vector<core::Session>& sessions_out,
             messaging::SubscriptionMap& sub_map_out);

} // namespace brokerino::persistence

#endif // BROKERINO_SRC_PERSISTENCE_RECOVERY_HPP

