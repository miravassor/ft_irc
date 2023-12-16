#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <map>
#include <vector>
#include <poll.h>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

#include "Client.hpp"
#include "Channel.hpp"

class Server {
    private:
        int socketFd;
        time_t start;
        sockaddr_in address;
        std::string password;
        std::string serverName;
        std::vector<pollfd> pollFds;
        std::map<int, Client *> clients;
        std::vector<Channel *> channels;

        void addClient(int clientSocket);
        void removeClient(int clientSocket);
        void listenPort() const;
        int acceptConnection();

        // Parsing
        char _buffer[1024];
        void    parsBuffer(int fd);
        void    registrationProcess(int fd, std::vector<std::string>& tokens);
        void    checkRegistration(int fd);
        void    processCmd(int fd, std::vector<std::string>& tokens);
        void    completeRegistration(int fd, Client *client);

    public:
        Server(int port, const std::string &password);
        ~Server();
        void run();
};

#endif
