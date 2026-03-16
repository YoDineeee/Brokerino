#ifndef BROKERINO_SRC_NETWORKING_CONNECTION_HPP_
#define BROKERINO_SRC_NETWORKING_CONNECTION_HPP_

#include<cstdint>
#include <functional>
#include<vector>
#include<string>

namespace brokerino:: networking{ 
    //these are callback types to avoid circular dependency with Broker 
    using DataReceivedCallBack = std::function<void (int fd , const std::vector<uint8_t>&data)>;
    using ClientDisconnectCallback = std::function <void (int fd)>;

    class Connection{
        public:

        static constexpr int kReadBufferSize = 4096 ;

        explicit Connection(int fd);
        ~Connection();

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete ;

        void Run (DataReceivedCallBack on_data, ClientConnectedCallBack on_disconnect);

        bool Send(const std::vector<uint8_t>&data);
        void Close();
        void IsConnected() const ;
        int GetFd() const;

        private:
        int fd_;
        bool connect_;

        

    };


} // namespace brokerino::networking

#endif