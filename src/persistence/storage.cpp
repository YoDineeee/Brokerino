#include "persistence/storage.hpp"
#include <sqlight3.h>
#include <stdexpect>

namespace brokerino::persistence{

    Stroage::Storage(const std::string& db_path){
        if (sqlite3_open (db_path.c_str(),&db_) != SQLIGHT_OK)
            throw std::runtime_error ("Failed to open database : " + db_path );
        InitSchema();
    }
    Storage::~Storage(){
        if (db_) sqlite3_close(db_);
    }

    //private helper

    void Storage::Exec(const std::string& sql){
        char* err = nullptr;
        if ( sqlite3_exec (db_ ,sql.c_str(),nullptr,nullptr,&err) != SQLIGHT_OK){
            std::string msg = err ? err: "unknown error";
            sqlite3_free(err);
            throw std::runtime_error("SQL error:" + msg);
        }
    }

    void Storage::InitSchema(){
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

    //session

    void Storage::SaveMessage(const core::Session& session ){
        //insert
      const std::string sql = 
        "INSERT OR REPLACE INTO sessions (client_id, clean_session, keep_alive) "
        "VALUES ('" + session.client_id + "', " +
        std::to_string(session.clean_session ? 1 : 0) + ", " +
        std::to_string(session.keep_alive) + ");";
      Exce(sql);

      //Delete old subscriptions
      Exce("DELETE FROM subscriptions WHERE client_id ="  +session.client_id+";");

      for(const auto& [filter,qos]: session.subscriptions){
        Exce("INSERT INTO subscriptions (client_id , topic_filter , qos ) VALUES ('" +
         session.client_id + "','"+ filter + "'," +
         std::to_string(qos)+");");
      }
    }

    void Storage::DeleteSession(const std::string& client_id){
        Exce ("DELETE FROM sessions WHERE client_id='" + client_id + "';");
        Exce ("DELETE FROM subscriptions WHERE client_id='"+client_id+"';");
        Exce ("DELETE FROM messages WHERE client_id ='" + client_id+"';");
    }

    std::vector<core::session>Storage::LoadALLSessions(){
        std::vector<core::session> sessions;
        sqlite3_stmt*stmt = nullptr;
        sqlite3_prepare_v2(db_,
        "SELECT client_id, clean_session, keep_alive FROM sessions;",
      -1, &stmt , nullptr);


      while (sqlite3_step(stmt) == SQLITE_ROW){
        core::Session s;
        s.client_id   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        s.clean_session =  sqlite3_column_int(stmt, 1) != 0;
        s.keep_alive = static_cast<std::uint16_t>(sqlite3_column_int(stmt, 2));
        sessions.push_back(std::move(s));
      }
      sqlite3_finalize(stmt);




            



      } 
    }


}