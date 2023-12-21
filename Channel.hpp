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
    Server *_server;
    std::string _name;
    std::string _topic;
    ChannelMode _mode;
  	std::string _password;
    std::string _visibility;
    std::set<int> _memberFds;
    std::set<int> _operatorFds;

public:

    Channel(const std::string &name, Server *server);

	Channel(const std::string &name, std::string &password, Server *server);

    ~Channel();


    const std::string& getName() const;

    const std::string& getTopic() const;

    const ChannelMode& getMode() const;

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

    bool authMember(int clientFd, std::string &password);

    // void broadcastMessage(int speakerFd, const std::string& message);

};


#endif
