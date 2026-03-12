#ifndef BROKERINO_SRC_AUTH_SIMPLE_AUTH_HPP
#define BROKERINO_SRC_AUTH_SIMPLE_AUTH_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace brokerino::auth{
    class SimpleAuth{
        public:
        void AddUser(const std::string& username , const std::string& password);
        void AddToken(const std::string& token);
        bool AuthenticateUser(const std::string& username , const std::string& password) const ;
        bool AuthenticateToken(const std::string& token );
        bool Authenticate(const std::string& token , const std::string& username, const std::string& password);

        private:
        std::unordered_map<std::string,std::string> users_;
        std::unordered_map<std::string> tokens_;
        

    }

}



#endif
