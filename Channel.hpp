#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>

enum ChannelMode {
    PRIVATE,
    PUBLIC
};

class Channel {

private:
    std::string _name;
    std::string _topic;
    ChannelMode _mode;
  	std::string _password;
    std::set<int> _memberFds;
    std::set<int> _operatorFds;

public:

    Channel(const std::string &name);

	Channel(const std::string &name, std::string &password);

    ~Channel();


    const std::string& getName() const;

    const std::string& getTopic() const;

    const ChannelMode& getMode() const;

    const std::set<int>& getMemberFds() const;

    const std::set<int>& getOperatorFds() const;


    void addMember(int clientFd);

    void removeMember(int clientFd);

    bool hasMember(int clientFd);

	bool authMember(int clientFd, std::string &password);

    void addOperator(int clientFd);

    void removeOperator(int clientFd);

    bool hasOperator(int clientFd);

    // void broadcastMessage(int speakerFd, const std::string& message);

};


#endif
