#ifndef  BROKERINO_SRC_PRESISTENCE_STORAGE_HPP_
#define BROKERINO_SRC_PRESISTENCE_STORAGE_HPP_

#include "core/mqtt_packet.hpp"
#include "core/mqtt_session.hpp"
#include<string>
#include<vector> 

struct sqlight3;

namespace brokerino::presistence{
    class Storage{
        public:
        explicit Storage(const std::string& db_path);
        ~Storage();

        Storage(const Storage&) = delete;
        Storage& operator = (const Storage&) = delete ;

        //sessions
        void SaveSession (const core::Session& session);
        void DeleteSession ( const core::string& client_id );

        std::vector<core::Session> LoadALLSessions();

        //message
        void SaveMessage(const std::string& client_id , const core::PublishPacket& packet );
        
        void DeleteMessage ( const std::string& client_id , std::uint16_t packet_id);

        std::vector<core::PublishPacket> LoadMessagesForClient(const std::string& client_id );

        private:

        void InitSchema();
        void Exce(const std::string& sql);

         sqlite3* db_ = nullptr;
    };
}


#endif