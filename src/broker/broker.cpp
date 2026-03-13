// the file handls the sessions,routing,lifecycle

#include "broker/broker.hpp"
#include "core/mqtt_codec.hpp"
#include "persistence / recovery.hpp"

#include <iostream>
#include <thread>

namespace brokerino{
    Broker::Broker(const std::string& db_path, int port)
        : server_(port)
        , storage_(db_path){
            //default credentials for MVP
            auth_.AddUser("admin","admin");
            auth_.AddToken("brokerino-default-token");

            //recover persistent sessions  on startup
            std::vector<core::Session> recovered;
            persistence::Recover(storage_,recovered,sub_map_);

            for (auto& session : recovered )
                std::cout<<"[Broker] Starting on port "<< server_.GetPort()<<"\n";
        }
    void Broker::Run(){
        std::cout<<"[Broker] starting on port"<<server_.GetPort() << "\n";
        server_.Run([this](int fd){OnClientConnected(fd);});

    }

    void Broker::Stop(){
        server_.Stop();
    }

    // Connection cycle 

    void Broker ::OnClientConnected(int fd){
         std::cout << "[Broker] New connection fd=" << fd << "\n";
         //connection
         auto conn = std::make_unique<networking::Connection>(fd);
         {
            std::lock_guard<std::mutex>lock(mutex_);
            connections_[fd] = std::move(conn);
            buffers_[fd] = {};
         }

         auto* conn_ptr = connections_[fd].get();

         std::thread([this , conn_ptr , fd](){
            conn_ptr->Run(
                [this,fd](const std::vector<std::uint8_t>& data){
                    OnData(fd,data);

                },
                [this,fd](){
                    OnDisconnect(fd);
                }
            );

         }) .detach();
    }

    void Broker::OnDisconnect(int fd){
        std::lock_guard<std::mutex> lock(mutex_);
        const std::string client_id = ClientIdForFd(fd);

        if (!client_id.empty()){
            auto it  = sessions_.find(fd);
            if (it != sessions_.end() && it -> second.clean_session){
                storage_.DeleteSession(client_id);
                sub_map_.RemoveClient(client_id);
            }
        }

        connections_.erase(fd);
        sessions_.erase(fd);
        buffers_.erase(fd);

        std::cout<<"[Broker] client disconnected  fd =" << fd << "\n"
    }

    //Data Handling

    void Broker::OnData(int fd ,const std::vector<std::uint8_t>& data ){
        std::lock_guard<std::mutex> lock(mutex_);

        auto& buf = buffers_[fd];
        buf.insert(buf.end() , data.begin() , data.end());

        while(true){
            auto result = core ::DecodeFixedHeader(buf);
            if (!result) break;

            const std::size_t total =
                result -> header_size + result -> header.remaining_length;

            if(buf.size() < total ) break ;

            const std::vector<std::uint8_t> payload(
                buf.begin() + result -> header_size,
                buf.begin() + total 
            );


            switch (result -> header.type){
                case core::PacketType::CONNECT:
                    HandleConnect(fd,payload);
                    break;

                case core::PacketType::PUBLISH:
                    HandlePublish(fd,result -> header , payload);
                    break;

                case core::PacketType::SUBSCRIBE:
                    HandleSubscribe(fd,payload);
                    break;

                case core::PacketType::PUBACK:
                    HandlePuback();
                    break;

                case core::PacketType::PINGREQ:
                    HandlePingreq();
                    break;

                case core::PacketType::DISCONNECTED:
                    HandleDisconnect(fd);
                    break;

                default:
                 std:cout<<"[Broker] Unknown packet on fd =" << fd << "\n";
                break;
            }
            buf.erase(buf.begin(), buf.begin() + total);
        }
    }

    //packet handlers

    void Broker::HandleConnect(int fd , const std::vector<std::uint8_t>& payload){
        auto packet = core::ParseConnect(payload);

    }


}