#include "persistence/storage.hpp"

#include <sqlite3.h>
#include <stdexcept>

namespace brokerino::persistence {

// ============================================================
// Construction / Destruction
// ============================================================

// Opens (or creates) the SQLite database and initializes schema.
Storage::Storage(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + db_path);
    }

    InitSchema();
}

// Closes the SQLite database connection.
Storage::~Storage() {
    if (db_) {
        sqlite3_close(db_);
    }
}

// ============================================================
// Internal Helpers
// ============================================================

// Executes a raw SQL statement.
// Intended for static queries (schema creation, etc.).
// Throws on failure.
void Storage::Exec(const std::string& sql) {
    char* err = nullptr;

    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown error";
        sqlite3_free(err);
        throw std::runtime_error("SQL error: " + msg);
    }
}

// Initializes required database tables.
void Storage::InitSchema() {

    // Session metadata
    Exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            client_id     TEXT PRIMARY KEY,
            clean_session INTEGER NOT NULL,
            keep_alive    INTEGER NOT NULL
        );
    )");

    // Subscriptions per client
    Exec(R"(
        CREATE TABLE IF NOT EXISTS subscriptions (
            client_id     TEXT NOT NULL,
            topic_filter  TEXT NOT NULL,
            qos           INTEGER NOT NULL,
            PRIMARY KEY (client_id, topic_filter)
        );
    )");

    // Stored QoS messages for retransmission
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

// ============================================================
// Session Persistence
// ============================================================

// Saves (or replaces) a session and all its subscriptions.
// Uses prepared statements to avoid SQL injection.
void Storage::SaveSession(const core::Session& session) {

    // Insert / replace session metadata
    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "INSERT OR REPLACE INTO sessions (client_id, clean_session, keep_alive) "
        "VALUES (?, ?, ?);",
        -1, &stmt, nullptr
    );

    sqlite3_bind_text(stmt, 1, session.client_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, session.clean_session ? 1 : 0);
    sqlite3_bind_int (stmt, 3, session.keep_alive);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Remove old subscriptions (overwrite strategy)
    sqlite3_prepare_v2(
        db_,
        "DELETE FROM subscriptions WHERE client_id = ?;",
        -1, &stmt, nullptr
    );

    sqlite3_bind_text(stmt, 1, session.client_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Insert new subscriptions
    sqlite3_prepare_v2(
        db_,
        "INSERT INTO subscriptions (client_id, topic_filter, qos) VALUES (?, ?, ?);",
        -1, &stmt, nullptr
    );

    for (const auto& [filter, qos] : session.subscriptions) {

        sqlite3_bind_text(stmt, 1, session.client_id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, filter.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int (stmt, 3, qos);

        sqlite3_step(stmt);
        sqlite3_reset(stmt); // reuse statement
    }

    sqlite3_finalize(stmt);
}

// Deletes a session and all associated data.
void Storage::DeleteSession(const std::string& client_id) {

    sqlite3_stmt* stmt = nullptr;

    const char* queries[] = {
        "DELETE FROM sessions WHERE client_id = ?;",
        "DELETE FROM subscriptions WHERE client_id = ?;",
        "DELETE FROM messages WHERE client_id = ?;"
    };

    for (const char* q : queries) {
        sqlite3_prepare_v2(db_, q, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, client_id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

// Loads all sessions and their subscriptions.
std::vector<core::Session> Storage::LoadAllSessions() {

    std::vector<core::Session> sessions;

    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "SELECT client_id, clean_session, keep_alive FROM sessions;",
        -1, &stmt, nullptr
    );

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        core::Session s;

        s.client_id =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        s.clean_session =
            sqlite3_column_int(stmt, 1) != 0;

        s.keep_alive =
            static_cast<std::uint16_t>(sqlite3_column_int(stmt, 2));

        sessions.push_back(std::move(s));
    }

    sqlite3_finalize(stmt);

    // Load subscriptions for each session
    sqlite3_stmt* sub_stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "SELECT topic_filter, qos FROM subscriptions WHERE client_id = ?;",
        -1, &sub_stmt, nullptr
    );

    for (auto& session : sessions) {

        sqlite3_bind_text(sub_stmt, 1, session.client_id.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(sub_stmt) == SQLITE_ROW) {

            std::string filter =
                reinterpret_cast<const char*>(sqlite3_column_text(sub_stmt, 0));

            std::uint8_t qos =
                static_cast<std::uint8_t>(sqlite3_column_int(sub_stmt, 1));

            session.subscriptions[filter] = qos;
        }

        sqlite3_reset(sub_stmt);
    }

    sqlite3_finalize(sub_stmt);

    return sessions;
}

// ============================================================
// Message Persistence (QoS Handling)
// ============================================================

// Stores a QoS message for retransmission.
void Storage::SaveMessage(const std::string& client_id,
                          const core::PublishPacket& packet) {

    // Only QoS > 0 packets have packet identifiers
    if (!packet.packet_id) return;

    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "INSERT OR REPLACE INTO messages "
        "(client_id, packet_id, topic, payload, qos, retain) "
        "VALUES (?, ?, ?, ?, ?, ?);",
        -1, &stmt, nullptr
    );

    sqlite3_bind_text(stmt, 1, client_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, *packet.packet_id);
    sqlite3_bind_text(stmt, 3, packet.topic.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 4,
                      packet.payload.data(),
                      static_cast<int>(packet.payload.size()),
                      SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 5, packet.qos);
    sqlite3_bind_int (stmt, 6, packet.retain ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Deletes a stored message after acknowledgment.
void Storage::DeleteMessage(const std::string& client_id,
                            std::uint16_t packet_id) {

    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "DELETE FROM messages WHERE client_id = ? AND packet_id = ?;",
        -1, &stmt, nullptr
    );

    sqlite3_bind_text(stmt, 1, client_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, packet_id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Loads stored messages for a client (used after reconnect).
std::vector<core::PublishPacket>
Storage::LoadMessagesForClient(const std::string& client_id) {

    std::vector<core::PublishPacket> packets;

    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(
        db_,
        "SELECT packet_id, topic, payload, qos, retain "
        "FROM messages WHERE client_id = ?;",
        -1, &stmt, nullptr
    );

    sqlite3_bind_text(stmt, 1, client_id.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        core::PublishPacket p;

        p.packet_id =
            static_cast<std::uint16_t>(sqlite3_column_int(stmt, 0));

        p.topic =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        const auto* blob =
            static_cast<const std::uint8_t*>(sqlite3_column_blob(stmt, 2));

        int size = sqlite3_column_bytes(stmt, 2);

        p.payload.assign(blob, blob + size);

        p.qos =
            static_cast<std::uint8_t>(sqlite3_column_int(stmt, 3));

        p.retain =
            sqlite3_column_int(stmt, 4) != 0;

        // Mark as duplicate for retransmission
        p.dup = true;

        packets.push_back(std::move(p));
    }

    sqlite3_finalize(stmt);

    return packets;
}

} // namespace brokerino::persistence

