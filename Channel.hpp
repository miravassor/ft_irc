#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>
#include <sstream>
#include <cstring>
#include <sys/socket.h>

#include "Server.hpp"

enum ChannelMode {
    PRIVATE,
    PUBLIC
};

enum chanRep {
    RPL_TOPIC,
    RPL_NOTOPIC,
    RPL_NAMREPLY
};

class Channel {

private:
    Server *server;
    std::string name;
    std::string topic;
    std::string mode;
    std::string visibility;
    std::set<int> memberFds;
    std::set<int> operatorFds;

public:

    Channel(const std::string &name, Server *server);

    ~Channel();


    const std::string& getName() const;

    const std::string& getTopic() const;

    const std::string& getMode() const;

    const std::set<int>& getMemberFds() const;

    const std::set<int>& getOperatorFds() const;


    void addMember(int clientFd);

    void removeMember(int clientFd);

    bool hasMember(int clientFd);

    void addOperator(int clientFd);

    void removeOperator(int clientFd);

    bool hasOperator(int clientFd);

    void newMember(int fd);

    void chanReply(int fd, chanRep id);

    void chanSendReply(int fd, std::string id, const std::string &token, const std::string &reply);

    // void broadcastMessage(int speakerFd, const std::string& message);

};


#endif
