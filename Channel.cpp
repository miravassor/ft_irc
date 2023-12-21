#include "Channel.hpp"

Channel::Channel(const std::string &name, Server *server) {
    this->_name = name;
	this->_password = "";
    this->server = server;
    this->visibility = "=";
	_mode = PUBLIC;
}

Channel::Channel(const std::string &name, std::string &password, Server *server) {
	this->_name = name;
	this->_password = password;
    this->server = server;
    this->visibility = "=";
	_mode = password.empty() ? PUBLIC : PRIVATE;
}

Channel::~Channel() {
}


const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

const ChannelMode& Channel::getMode() const {
    return _mode;
}

const std::set<int>& Channel::getMemberFds() const {
    return _memberFds;
}

const std::set<int>& Channel::getOperatorFds() const {
    return _operatorFds;
}


void Channel::addMember(int clientFd) {
    _memberFds.insert(clientFd);
}

void Channel::removeMember(int clientFd) {
    removeOperator(clientFd);
    _memberFds.erase(clientFd);
}

bool Channel::hasMember(int clientFd) {
    return memberFds.find(clientFd) != memberFds.end();
}

bool Channel::authMember(int clientFd, std::string &password) {
	if (_mode == PRIVATE && password != _password) {
		return false;
	}
	addMember(clientFd);
	return true;
}

void Channel::addOperator(int clientFd) {
    _operatorFds.insert(clientFd);
}


void Channel::removeOperator(int clientFd) {
    _operatorFds.erase(clientFd);
}

bool Channel::hasOperator(int clientFd) {
    return _operatorFds.find(clientFd) != _operatorFds.end();
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
//     std::string fullMessage = "[" + this->_name + "] " + message + "\r\n";
//     // Broadcast the message to all members
//     for (int clientFd : clientFds) {
//         ssize_t bytesSent = send(clientFd, fullMessage.c_str(), fullMessage.length(), 0);
//         if (bytesSent < fullMessage.length()) {
//             ;
//         // Close the connection and remove client
//         }
//     }

// }
