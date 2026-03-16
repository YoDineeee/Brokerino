#include "auth/simple_auth.cpp"

namespace brokerino::auth{
    void SimpleAuth::AddUser(const std::string& username , const std::string& password){
        users_[username] = password ;

    }

    void SimpleAuth::AddToken(const std::string& token ){
        tokens_.insert(token);
    }

    bool SimpleAuth::AuthenticateUser(const std::string& username , const std::string& password ) const {
        auto it = users_.find(username);
        if(it == users_.end()) return false;
        return it -> second == password;

    }


    bool SimpleAuth::AuthenticateToken(const std::string& token )const{
        return tokens_.count(token)>0;

    }


    bool SimpleAuth:: Authenticate(const std::string& username , const std::string& password , const std::string& token )const{
        if ( ! token.empty() && AuthenticateToken( token )) return true;
        if (! username.empty() && AuthenticateUser(username ,password )) return true ;
        return false;
    }



}  // namespace brokerino::auth