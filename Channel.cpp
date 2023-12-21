#include "Channel.hpp"

Channel::Channel(const std::string &name) {
    this->_name = name;
	this->_password = "";
	_mode = PUBLIC;
}

Channel::Channel(const std::string &name, std::string &password) {
	this->_name = name;
	this->_password = password;
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
    return _memberFds.find(clientFd) != _memberFds.end();
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


