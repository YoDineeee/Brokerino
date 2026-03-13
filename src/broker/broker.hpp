#ifndef BROKERINO_SRC_BROKER_BROKER_HPP_
#define BROKERINO_SRC_BROKER_BROKER_HPP_

#include "networking / tcp_server.hpp"
#include "networking / connection.hpp"
#include "core/mqtt_packet.hpp"
#include "core/mqtt_session.hpp"
#include "message / subscription_map.hpp"
#include "persistence / storage.hpp"
#include "auth / simple_auth.hpp"

#include<unordered_map>
#include <string>
#include<mutex> 
#include <memory>
#include<vector>
#include<cstdint>



namespace brokerino{
    class Broker{
        public:
            explicit Broker (const std::string& db_path, int port);
            ~Broker() = default;

            Broker(const Broker&) = delete;
            Broker& operator =(const Broker& ) = delete;

            void Run();
            void Stop();
        private:
            void OnClientConnected(int fd);
            void OnData(int fd , const std::vector<std::uint8_t>& data);
            void OnDisconnect(int fd);
            //packet handler
            void HandleConnect(int fd, const std::vector<std::uint8_t>& payload);
            void HandlePublish(int fd, const core::FixedHeader& header ,const std::vector<std::uint8_t>& payload);
            void HandleSubscribe(int fd, const std::vector<std::uint8_t>& payload);
            void HandlePuback(int fd, const std::vector<std::uint8_t>& payload);
            void HandlePingreq(int fd);
            void HandleDisconnect(int fd);

            // helper
            void SendToClient(int fd , const std::vector<std::uint8_t>& data );
            std::string ClientIdForFd(int fd)const ;

            //subsystems
            networking::TcpServer server_;
            message::SubscriptionMap sub_map_;
            persistence::storage storage_ ;
            auth::SimpleAuth auth_;

            //state
            std::unordered_map<int , std::vector<std::uint8_t>> buffers_;
            std::unordered_map<int, core::Session> sessions_;
            std::unordered_map<int , std::unique_ptr<networking::Connection>> connections_;

            std::mutex mutex_;

    };
}


#endif