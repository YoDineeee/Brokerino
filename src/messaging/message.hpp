#ifndef BROKERINO_SRC_MESSAGE_MESSAGING_HPP
#define BROKERINO_SRC_MESSAGE_MESSAGING_HPP


#include<cstdint>
#include<string>
#include<vector>

namespace brokerino::messaging{
    struct Message{
        std::string topic;
        std::vector <std::uint8_t> payload;
        std::uint8_t qos;
        bool retain;

    };
}// namespace brokerino::messaging




#endif