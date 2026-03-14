#ifndef BROKERINO_SRC_MESSAGE_MESSAGE_HPP_
#define BROKERINO_SRC_MESSAGE_MESSAGE_HPP_


#include<cstdint>
#include<string>
#include<vector>

namespace brokerino::message{
    struct Message{
        std::string topic;
        std::vector <std::uint8_t> payload;
        std::uint8_t qos;
        bool retain;

    };
}




#endif