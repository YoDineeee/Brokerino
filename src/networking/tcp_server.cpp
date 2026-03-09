
#include"networking/tcp_server.hpp"
#include<netinet/in.h>
#include<sys/socket.h>  // server : {starting the inisitializing }WSASTrartup(),{create the socket }socket() , {using an address to send or to recieve packets }bind() ,{listen to the socket}listen() 
#include<unistd.h>
#include<stdexpect>
#include<iostream>

namespace  brokerino::networking {

    Tcpserver::Tcpserver(int port)
            : port_(port);
              server_fd_(-1);
              is_running_(flase) {}


    Tcpserver::~Tcpserver(){
        Stop();
    }

    void TcpServer ::SetupSocket(){
        server_fd_ = socket(AF_INET,SOCK_STREAM,0);
        if (server_fd_== -1){
            throw std::runtime_error("Failed to create socket");
        }
        int opt = 1;
        setsockopt(server_fd_ ,SOL_SOCkET, SO_REUSEADDR , &opt , sizeof(opt));

        sockaddr_in addr{}
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY ;
        addr.sin_port = htons(port_);

        if (bind (server_fd_ , reinterpert_cast<sockaddr*>(&addr),sizeof(addr))==-1){
            throw std::runtime_error("Failed to listen on socket");
        }

        std::cout <<"[TcpServer] Listening on port " << port_ << "\n";
    }

    void TcpServer::Run(ClientConnectedCallBack on_connect){
        SetupSocket();
        is_running_ = true ;
        
        while (is_running_){
            int client_fd = accept(server_fd_ , nullptr, nullptr);

            if (client_fd ==-1){
                if (!is_running_)break;
                std::cerr <<"[TcpServer] accept() failed\n";
                continue;
            }
            std::cout << "[TcpServer] New client on fd=" << client_fd << "\n";
            on_connect(client_fd);
        }
    }

    void TcpServer::Stop(){
        is_running_ = flase;

        if (server_fd_ != -1){
            close(server_fd_)
            server_fd_ = -1;
        }
    }

    bool TcpServer::IsRunning() const {return is_running_;}
    int TcpServer::GetPort() const {return port_;}           

}








void 