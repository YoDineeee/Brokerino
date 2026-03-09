#ifndef BROKERINO_SRC_NETWORKING_TCP_SERVER_HPP
#define BROKERINO_SRC_NETWORKING_TCP_SERVER_HPP
// C++ header files
#include <functional> //using this because if not i will be inside of a trap called "circular dependency" with broker , connection
#include <string>

namespace brokerino::networking{
    using ClientConnectedCallBack = std::function <void(int fb)>;

    class TcpServer{
        public:
            //why using int not unit16_t ..cuz mixing int and uint16_t will make this annoying (making it simple)
            // why using constexpr is better then const ..keeps the pomise to the compiler ( so the value is fixed at compile time )
            //why using static? ..cuz lets say that we have multiple servers connect to the same port..to solve this using (static under the variable kDefaultPort)
            static constexpr int kDefaultPort = 1883;
            static constexpr int kBacklog = 10;

            //if i dont use explicit so it will not convert to tcpserver automatically + Google rule  +  this constructor has default argument that means it can be returned with zero argument 
            explicit Tcpserver (int Port = kDefaultPort);
            ~Tcpserver();
          
            Tcpserver(const Tcpserver&) = delete ;
            Tcpserver& operator = (const Tcpserver& ) = delete ;

            void Run (ClientConnectedCallBack on_connect);
            void Stop ();


            bool IsRunning() const ;
            int GetPort() const;

        private:
        
            void SetupSocket();
            int port_,
            int server_fd_,
            bool is_running_ = false ;

    };
    

} // namespace brokerino::networking

#endif
