#include "Channel.hpp"

Channel::Channel(const std::string &name) {
    this->name = name;
    std::cout << "Channel " << this->name << " created." << std::endl;
}

Channel::~Channel() {
    std::cout << "Channel " << this->name << " destroyed." << std::endl;
}


const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getTopic() const {
    return topic;
}

const std::string& Channel::getMode() const {
    return mode;
}

const std::set<int>& Channel::getMemberFds() const {
    return memberFds;
}

const std::set<int>& Channel::getOperatorFds() const {
    return operatorFds;
}


void Channel::addMember(int clientFd) {
    memberFds.insert(clientFd);
}

void Channel::removeMember(int clientFd) {
    removeOperator(clientFd);
    memberFds.erase(clientFd);
}

bool Channel::hasMember(int clientFd) {
    return memberFds.find(clientFd) != memberFds.end();
}


void Channel::addOperator(int clientFd) {
    operatorFds.insert(clientFd);
}


void Channel::removeOperator(int clientFd) {
    operatorFds.erase(clientFd);
}

bool Channel::hasOperator(int clientFd) {
    return operatorFds.find(clientFd) != operatorFds.end();
}

// Channel::void broadcastMessage(int speakerFd, const std::string& message) {
//     std::string fullMessage = "[" + this->name + "] " + message + "\r\n";
//     // Broadcast the message to all members
//     for (int clientFd : clientFds) {
//         ssize_t bytesSent = send(clientFd, fullMessage.c_str(), fullMessage.length(), 0);
//         if (bytesSent < fullMessage.length()) {
//             ;
//         // Close the connection and remove client
//         }
//     }

// }


