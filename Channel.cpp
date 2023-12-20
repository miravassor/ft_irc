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

void    Channel::newMember(int fd) {
    if (!getTopic().empty()) {
        chanReply(fd, RPL_TOPIC);
    }
    else {
        chanReply(fd, RPL_NOTOPIC);
    }
    chanReply(fd, RPL_NAMREPLY);
}

void    Channel::chanReply(int fd, chanRep id) {
    (void)fd;
    switch (id) {
        case RPL_TOPIC:
		// to do: replace <topic> with real value
            // send(fd, replyStr.c_str(), replyStr.length(), 0);
			// serverSendReply(fd, "332", "<topic>");
			break;
		case RPL_NOTOPIC:
			// serverSendReply(fd, "331", "No topic is set");
			break;
		case RPL_NAMREPLY:
		// to do: replace <nick> with real value
			// serverSendReply(fd, "353", "<nick>");
			break;
    }
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


