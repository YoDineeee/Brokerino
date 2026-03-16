#include "networking/connection.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdint>

namespace brokerino::networking{

    Connection::Connection(int fd)
            :fd_(fd),
            connected_(fd != -1){}


    Connection :: ~Connection(){
        Close();
    }

    void Connection::Run (DataReceivedCallBack on_data , ClientDisconnectCallback on_disconnect){
        std::vector<uint8_t> buf(kReadBufferSize);
        while(connected_){
            ssize_t bytes = recv(fd_,buf.data(), buf.size(),0); 
           if (bytes <= 0){
              if (bytes == -1)
                std::cerr << "[Connection] recv() error on fd=" << fd_
                          << " : " << strerror(errno) << "\n";
              break;
           }
        }
        on_data(fd_, {buf.begin(), buf.begin() + bytes});
    }
     Close();
    on_disconnect(fd_);



    bool Connection::Send(const std::vector<uint8_t>& data ){
        if (!connected_) return false ;

        ssize_t sent = send(fd_,data.data(),data.size(),0);
        if (sent == -1){
            std::cerr <<"[Connection] send() error  on fd="<< fd_
                      << ":"<< strerror(errno)<< "\n";
                return false ;
        };
        return true;
    }

    void Connection::Close(){

    }

    bool Connetion::IsConnected() const {return connected_;}
    int  Connection::GetFd() const {return fd_;}
} // namespace brokerino::networking