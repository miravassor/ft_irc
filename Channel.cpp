#include "Channel.hpp"

Channel::Channel(const std::string &name, Server *server) {
    this->name = name;
    this->server = server;
    this->visibility = "=";
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
    switch (id) {
        case RPL_TOPIC:
		    // to do: replace <topic> with real value
            // send(fd, replyStr.c_str(), replyStr.length(), 0);
			// serverSendReply(fd, "332", "<topic>");
			break;
		case RPL_NOTOPIC:
			chanSendReply(fd, "331", server->getNick(fd) + " " + name, "No topic is set");
			break;
		case RPL_NAMREPLY:
            std::string nmrp = ":" + server->getServerName() + " " + server->getNick(fd) + " 353 " + visibility + " ";
            std::set<int>::iterator it = memberFds.begin();
            for (; it != memberFds.end(); ++it) {
                nmrp.append(server->getNick(fd));
                std::set<int>::iterator nextIt = it;
                ++nextIt;
                if (nextIt != memberFds.end())
                    nmrp.append(" ");
            }
            send(fd, nmrp.c_str(), nmrp.length(), 0);
			break;
    }
}

void    Channel::chanSendReply(int fd, std::string id, const std::string &token, const std::string &reply) {
    std::stringstream fullReply;
	fullReply << ":" << server->getServerName() << " " << id << " " << token << " :" << reply << "\r\n";
	std::string replyStr = fullReply.str();
	send(fd, replyStr.c_str(), replyStr.length(), 0);
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


