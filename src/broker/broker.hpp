#ifndef BROKERINO_SRC_BROKER_BROKER_HPP_
#define BROKERINO_SRC_BROKER_BROKER_HPP_

#include "networking / tcp_server.hpp"
#include "core/mqtt_packet.hpp"
#include "core/mqtt_session.hpp"
#include "message / subscription_map.hpp"
#include "persistence / storage.hpp"
#include "auth / simple_auth.hpp"

#include<unordered_map>
#include <string>
#include<mutex> 

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
        void 

    }
}


#endif