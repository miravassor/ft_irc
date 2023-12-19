#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

class Client {
    private:
        int         socketFd;
        bool        logged;
        bool        registered;
        bool        isOpe;
        std::string nickname;
        std::string username;
        std::string realName;
        std::string password;
    public:
        Client();
        ~Client();
        const std::string &getNickname() const;
        const std::string &getUsername() const;
        const std::string &getPassword() const;
        int getSocket();
        bool isRegistered();
        bool isOperator();
        bool isLogged();
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setPassword(const std::string &password);
        void setRegistration();
        void setLog();
};

#endif
