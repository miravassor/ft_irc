#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "Client.hpp"
#include <map>
#include <vector>
#include <poll.h>

class Server {

private:

    int socketFd;
    sockaddr_in address;
    std::string password;
    std::vector<pollfd> pollFds;
    std::map<int, Client *> clients;

    void addClient(int clientSocket);

    bool removeClient(int clientSocket);

    bool removeClient(const std::string &nickname);

    void listenPort() const;

    int acceptConnection();

public:

    Server(int port, const std::string &password);

    ~Server();

    void run();

};

#endif
