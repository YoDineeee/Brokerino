#include "persistence/storage.hpp"
#include <sqlite3.h>
#include <stdexcept>

namespace brokerino::persistence {

Storage::Storage(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK)
        throw std::runtime_error("Failed to open database: " + db_path);
    InitSchema();
}

Storage::~Storage() {
    if (db_) sqlite3_close(db_);
}

//  Private helpers 

void Storage::Exec(const std::string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown error";
        sqlite3_free(err);
        throw std::runtime_error("SQL error: " + msg);
    }
}

void Storage::InitSchema() {
    Exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            client_id     TEXT PRIMARY KEY,
            clean_session INTEGER NOT NULL,
            keep_alive    INTEGER NOT NULL
        );
    )");

    Exec(R"(
        CREATE TABLE IF NOT EXISTS subscriptions (
            client_id     TEXT NOT NULL,
            topic_filter  TEXT NOT NULL,
            qos           INTEGER NOT NULL,
            PRIMARY KEY (client_id, topic_filter)
        );
    )");

    Exec(R"(
        CREATE TABLE IF NOT EXISTS messages (
            client_id  TEXT    NOT NULL,
            packet_id  INTEGER NOT NULL,
            topic      TEXT    NOT NULL,
            payload    BLOB    NOT NULL,
            qos        INTEGER NOT NULL,
            retain     INTEGER NOT NULL,
            PRIMARY KEY (client_id, packet_id)
        );
    )");
}

// Sessions 

void Storage::SaveSession(const core::Session& session) {
    const std::string sql =
        "INSERT OR REPLACE INTO sessions (client_id, clean_session, keep_alive) "
        "VALUES ('" + session.client_id + "', " +
        std::to_string(session.clean_session ? 1 : 0) + ", " +
        std::to_string(session.keep_alive) + ");";
    Exec(sql);

    Exec("DELETE FROM subscriptions WHERE client_id='" + session.client_id + "';");

    for (const auto& [filter, qos] : session.subscriptions) {
        Exec("INSERT INTO subscriptions (client_id, topic_filter, qos) VALUES ('" +
             session.client_id + "', '" + filter + "', " +
             std::to_string(qos) + ");");
    }
}

void Storage::DeleteSession(const std::string& client_id) {
    Exec("DELETE FROM sessions      WHERE client_id='" + client_id + "';");
    Exec("DELETE FROM subscriptions WHERE client_id='" + client_id + "';");
    Exec("DELETE FROM messages      WHERE client_id='" + client_id + "';");
}

std::vector<core::Session> Storage::LoadAllSessions() {
    std::vector<core::Session> sessions;

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_,
        "SELECT client_id, clean_session, keep_alive FROM sessions;",
        -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        core::Session s;
        s.client_id     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        s.clean_session = sqlite3_column_int(stmt, 1) != 0;
        s.keep_alive    = static_cast<std::uint16_t>(sqlite3_column_int(stmt, 2));
        sessions.push_back(std::move(s));
    }
    sqlite3_finalize(stmt);

    for (auto& session : sessions) {
        sqlite3_stmt* sub_stmt = nullptr;
        sqlite3_prepare_v2(db_,
            ("SELECT topic_filter, qos FROM subscriptions WHERE client_id='" +
             session.client_id + "';").c_str(),
            -1, &sub_stmt, nullptr);

        while (sqlite3_step(sub_stmt) == SQLITE_ROW) {
            std::string filter =
                reinterpret_cast<const char*>(sqlite3_column_text(sub_stmt, 0));
            std::uint8_t qos =
                static_cast<std::uint8_t>(sqlite3_column_int(sub_stmt, 1));
            session.subscriptions[filter] = qos;
        }
        sqlite3_finalize(sub_stmt);
    }

    return sessions;
}

//  Messages 

void Storage::SaveMessage(const std::string& client_id,
                           const core::PublishPacket& packet) {
    if (!packet.packet_id) return;

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_,
        "INSERT OR REPLACE INTO messages "
        "(client_id, packet_id, topic, payload, qos, retain) "
        "VALUES (?, ?, ?, ?, ?, ?);",
        -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, client_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 2, *packet.packet_id);
    sqlite3_bind_text(stmt, 3, packet.topic.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, packet.payload.data(),
                      static_cast<int>(packet.payload.size()), SQLITE_STATIC);
    sqlite3_bind_int (stmt, 5, packet.qos);
    sqlite3_bind_int (stmt, 6, packet.retain ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Storage::DeleteMessage(const std::string& client_id,
                             std::uint16_t packet_id) {
    Exec("DELETE FROM messages WHERE client_id='" + client_id +
         "' AND packet_id=" + std::to_string(packet_id) + ";");
}

std::vector<core::PublishPacket> Storage::LoadMessagesForClient(
        const std::string& client_id) {
    std::vector<core::PublishPacket> packets;

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_,
        ("SELECT packet_id, topic, payload, qos, retain FROM messages "
         "WHERE client_id='" + client_id + "';").c_str(),
        -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        core::PublishPacket p;
        p.packet_id = static_cast<std::uint16_t>(sqlite3_column_int(stmt, 0));
        p.topic     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        const auto* blob = static_cast<const std::uint8_t*>(
                               sqlite3_column_blob(stmt, 2));
        const int   size = sqlite3_column_bytes(stmt, 2);
        p.payload.assign(blob, blob + size);

        p.qos    = static_cast<std::uint8_t>(sqlite3_column_int(stmt, 3));
        p.retain = sqlite3_column_int(stmt, 4) != 0;
        p.dup    = true;

        packets.push_back(std::move(p));
    }
    sqlite3_finalize(stmt);

    return packets;
}

} // namespace brokerino::persistence