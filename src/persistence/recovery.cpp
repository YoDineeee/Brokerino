#include "persistence/recovery.hpp"

namespace brokerino::persistence {

// ============================================================
// Session Recovery
// ============================================================

// Restores broker state from persistent storage.
//
// This function:
// 1. Loads all previously stored sessions.
// 2. Rebuilds the in-memory subscription map for non-clean sessions.
//
// MQTT rule:
// - Sessions with `clean_session = true` MUST NOT be restored.
// - Only persistent sessions (clean_session = false) retain subscriptions.
void Recover(Storage&                    storage,
             std::vector<core::Session>& sessions_out,
             messaging::SubscriptionMap& sub_map_out) {

    // Load all sessions from persistent storage
    sessions_out = storage.LoadAllSessions();

    // Rebuild subscription map from stored sessions
    for (const auto& session : sessions_out) {

        // Skip clean sessions (they should not persist state)
        if (session.clean_session)
            continue;

        // Re-register all subscriptions for this session
        for (const auto& [filter, qos] : session.subscriptions) {
            sub_map_out.Subscribe(session.client_id, filter, qos);
        }
    }
}

} // namespace brokerino::persistence

