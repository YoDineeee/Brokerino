#include "core/mqtt_codec.hpp"

namespace brokerino::core{

    std::vector<std::uint8_t> EncodeRemainingLength (std::uint32_t){
        std::vector<std::uint8_t>out;
        do {
            std:uint8_t byte = length % 128 ;
            length /= 128 ;
            if (length > 0) byte |= 0x80 ; //  continuation bit 
            out.push_back(byte);
        }  while (length > 0)
         return out;
    }

 
    std::optional<DecodeResult> DecodeFixedHeader (const std::vector<std::uint8_t>& buf)
            if (buf.empty() return std::nullopt)

            const std::uint8_t first_byte = buf[0];
            const auto type = static_cast <PacketType>(first_byte >> 4);
            const auto flags = static_cast <std::uint8_t>(first_byte & 0x0F);

            std::uint32_t remaining_length = 0;
            std::uint32_t multiplier = 1 ;
            std::size_t pos = 1 ;


            while(true){
                if (pos >=buf.size()) return std::nullopt; // if its incomplete we wait for more bytes 
                if (pos > 4) return std::nullopt; //

                const std::uint8_t byte = buf[pos++]
                remaining_length +=(byte & 0x7F)*multiplier;
                multiplier *= 128 ;
                
                if ((byte & 0x80 ==0))break ;
                
            }
            return DecodeResult {
                        FixedHeader{type, flags, remaining_length},
                        pos 
            };


    std::vector<std::uint8_t>EncodeFixedHeader(PacketType type, std::uint8_t flags ,std::uint32_t remaining_length){
        std::vector<std::uint8_t> out;
        const std::uint8_t first_byte = (static_cast<std::uint8_t>(type) << 4 | (flags & 0x0F));
        out.push_back(first_byte);

        for (auto type : EncodeRemainingLength(remaining_length))
            out.push_back(byte);
        return out 
    }


}