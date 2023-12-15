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

    std::string nickname;

public:

    Client();

    ~Client();

    const std::string &getNickname() const;

    void setNickname(const std::string &nickname);

};

#endif
