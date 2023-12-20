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
    std::string name;
    std::string topic;
    std::string mode;
    std::set<int> memberFds;
    std::set<int> operatorFds;

public:

    Channel(const std::string &name);

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

    // void broadcastMessage(int speakerFd, const std::string& message);

};


#endif
