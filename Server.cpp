#include "Server.hpp"

Server::Server(int port, const std::string &password) {
    // setting the address family - AF_INET for IPv4
    address.sin_family = AF_INET;
    // setting the port converting port value to network byte order
    address.sin_port = htons(port);
    // setting the IP - INADDR_ANY for any network interface on the machine - converting it to network byte order.
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // creating the main listening socket and adding it to pollFds container
    socketFd = socket(address.sin_family, SOCK_STREAM, 0);
    if (socketFd == -1) {
        throw std::runtime_error("ERROR! Cannot create socket");
    }
    pollfd serverPollFd;
    serverPollFd.fd = socketFd;
    serverPollFd.events = POLLIN;
    pollFds.push_back(serverPollFd);

    // binding socket to the port
    if (bind(this->socketFd, (sockaddr *) (&address), sizeof(address)) == -1) {
        throw std::runtime_error("ERROR! Cannot bind the socket");
    }

    this->start = time(0);
    this->password = password;
    this->serverName = "42.IRC";
    this->serverVersion = "0.1";

	initCmd();

    std::cout << "Server created: address=" << inet_ntoa(address.sin_addr)
              << ":"
              << ntohs(address.sin_port)
              << " socketFD=" << socketFd
              << " password=" << this->password << std::endl;
}

void Server::initCmd() {
	cmd["PRIVMSG"] = &Server::processPrivmsg;
	cmd["JOIN"] = &Server::processJoin;
	cmd["INVITE"] = &Server::processInvite;
	cmd["KICK"] = &Server::processKick;
	cmd["PART"] = &Server::processPart;
	// and other commands
}

Server::~Server() {
    std::cout << "Server destroyed." << std::endl;
}


void Server::addClient(int clientSocket) {
    // Create a new Client object and insert it into the clients map
    clients.insert(std::make_pair(clientSocket, new Client()));
}

void Server::removeClient(int clientSocket) {
    // removing from channels
   for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        (*it)->removeMember(clientSocket);
    }
    // removing from pollFds
   for (std::vector<pollfd>::iterator it = pollFds.begin(); it!= pollFds.end(); ++it) {
        if (it->fd == clientSocket) {
            pollFds.erase(it);
            break;
        }
    }
    // deleting and removing from clients
    std::map<int, Client *>::iterator it = clients.find(clientSocket);
    if (it != clients.end()) {
        delete it->second;
        clients.erase(it);
    }
}


void Server::run() {

    listenPort();

    while (true) {
        int countEvents = poll(&pollFds[0], pollFds.size(), -1);

        if (countEvents < 0) {
            throw std::runtime_error("ERROR! Polling error");
        }
        for (size_t i = 0; i < pollFds.size(); i++) {
            if (pollFds[i].revents & POLLIN) {
                // if i == 0 -> first connection
                if (i == 0) {
                    addClient(acceptConnection());
                } else {
                    std::cout << "Client socket has events!" << std::endl;

                    memset(_buffer, 0, 1024);
                    int bytesRead = recv(pollFds[i].fd, _buffer, sizeof(_buffer) - 1, 0);
                    if (bytesRead > 0) {
                        _buffer[bytesRead] = 0;
                        parsBuffer(pollFds[i].fd);
                        // if (parsBuffer(pollFds[i].fd)) {
                        //     removeClient(pollFds[i].fd);
                        //     i--;
                        // }
                    } else if (bytesRead == 0) {
                        removeClient(pollFds[i].fd);
                        i--;
                    } else {
                        throw std::runtime_error("SOME TMP ERROR");
                    }
                    pollFds[i].revents = 0;
                }
            }
        }
    }
}

void Server::listenPort() const {

    if (listen(socketFd, SOMAXCONN) == -1) {
        throw std::runtime_error("ERROR! Cannot listen on the socket");
    }
    std::cout << "Server is listening for incoming connections" << std::endl;
}

int Server::acceptConnection() {
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    pollfd clientPollFd;

    // accept connection and dd the new client's socket to the pollFds container
    int clientSocket = accept(socketFd, (sockaddr *) (&clientAddress), &clientAddressLength);
    if (clientSocket == -1) {
        throw std::runtime_error("ERROR! Cannot accept the connection");
    }
    clientPollFd.fd = clientSocket;
    clientPollFd.events = POLLIN;
    pollFds.push_back(clientPollFd);

    // print information about the accepted connection
    std::cout << "Accepted connection from: " << inet_ntoa(clientAddress.sin_addr) << ":"
              << ntohs(clientAddress.sin_port)
              << " at fd=" << clientSocket << std::endl;

    return clientSocket;
}
