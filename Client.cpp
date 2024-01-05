#include "Client.hpp"
#include "Server.hpp"
// Build

Client::Client(int socket)
	: socketFd(socket),
	  logged(false),
	  registered(false),
	  isOpe(false),
	  modes(0),
	  recvBuffer(""),
	  _awayMessage(""),
	  _quit(
		  false) {
	std::cout << "Client created" << std::endl;
}

Client::~Client() {
	std::cout << "Client destroyed." << std::endl;
}

// Setters

void Client::setNickname(const std::string &nickname) {
	this->nickname =  Server::uncapitalizeString(nickname);
}

void Client::setUsername(const std::string &username) {
	Client::username = username;
}

void Client::setLog() {
	Client::logged = true;
}

void Client::setRegistration() {
	Client::registered = true;
}

void Client::setPassword(const std::string &password) {
	Client::password = password;
}

void Client::setAwayMessage(const std::string &away) {
	Client::_awayMessage = away;
}

// Getters

const std::string &Client::getNickname() const {
	return nickname;
}

const std::string &Client::getUsername() const {
	return username;
}

bool Client::isRegistered() {
	return registered;
}

bool Client::isOperator() {
	return isOpe;
}

bool Client::isLogged() {
	return logged;
}

int Client::getSocket() {
	return socketFd;
}

const std::string &Client::getPassword() const {
	return password;
}

const std::string Client::getRealName() const {
    return username;
}

void Client::pushSendQueue(std::string send) {
	this->_sendQueue.push(send);
}

std::string Client::popSendQueue() {
	std::string ret(this->_sendQueue.front());
	this->_sendQueue.pop();
	return ret;
}

bool Client::sendQueueEmpty() {
	return this->_sendQueue.empty();
}

void Client::appendRecvBuffer(std::string recv) {
	this->recvBuffer.append(recv);
}

std::string Client::getRecvBuffer() {
	return this->recvBuffer;
}

void Client::resetRecvBuffer() {
	this->recvBuffer.clear();
}

bool Client::isRecvBufferEmpty() {
	return this->recvBuffer.empty();
}

const std::string &Client::getAwayMessage() const {
	return _awayMessage;
}

const std::vector<std::string> &Client::getChannels() const {
	return _channels;
}

void Client::addChannel(const std::string &channel) {
	_channels.push_back(channel);
}

void Client::removeChannel(const std::string &channel) {
	for (std::vector<std::string>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it == channel) {
			_channels.erase(it);
			break;
		}
	}
}

bool    Client::isInChannel(const std::string &channel) {
    std::vector<std::string>::iterator it = _channels.begin();
    for (; it != _channels.end(); ++it) {
        if (channel == *it)
            return true;
    }
    return false;
}

std::string Client::returnModes() {
	std::string fullModes;

	if (activeMode(AWAY))
		fullModes.append("a");
	if (activeMode(INVISIBLE))
		fullModes.append("i");
//    if (activeMode(OPERATOR))
//        fullModes.append("o");
//    if (activeMode(RESTRICTED))
//        fullModes.append("r");
	if (!fullModes.empty()) {
		fullModes.insert(0, "+");
		return fullModes;
	} else
		return "";
}
bool Client::isQuit() const {
	return _quit;
}
void Client::setQuit(bool quit) {
	Client::_quit = quit;
}

