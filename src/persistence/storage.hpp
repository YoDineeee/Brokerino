/**
 * @file storage.hpp
 * @brief Persistent storage layer using SQLite.
 *
 * Handles saving and loading of MQTT sessions and QoS messages so that
 * clients (especially the Telegram bot) can recover state after restart.
 */



#ifndef BROKERINO_SRC_PERSISTENCE_STORAGE_HPP
#define BROKERINO_SRC_PERSISTENCE_STORAGE_HPP

#include "core/mqtt_packet.hpp"
#include "session/mqtt_session.hpp"
#include <string>
#include <vector>

struct sqlite3;

namespace brokerino::persistence {

    /**
 * @class Storage
 * @brief SQLite-based persistence for sessions and QoS messages.
 *
 * This class is responsible for:
 * - Saving client sessions (subscriptions + clean_session flag)
 * - Storing QoS 1 messages for later retransmission
 * - Loading everything back on broker startup (see recovery.hpp)
 */


class Storage {
public:
/**
     * @brief Opens (or creates) the SQLite database.
     * @param db_path Path to the database file (e.g. "brokerino.db")
     */
     
    explicit Storage(const std::string& db_path);
    ~Storage();

    Storage(const Storage&)            = delete;
    Storage& operator=(const Storage&) = delete;

    // Sessions
    void SaveSession  (const core::Session& session);
    void DeleteSession(const std::string& client_id);
    std::vector<core::Session> LoadAllSessions();

    // Messages
    void SaveMessage (const std::string& client_id,const core::PublishPacket& packet);
    void DeleteMessage (const std::string& client_id,  std::uint16_t packet_id);
    std::vector<core::PublishPacket>  LoadMessagesForClient(const std::string& client_id);

private:
    void InitSchema();
    void Exec(const std::string& sql);

    sqlite3* db_ = nullptr;
};

} // namespace brokerino::persistence

#endif