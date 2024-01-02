#include "Client.hpp"

// Build

Client::Client(int socket)
		: socketFd(socket), logged(false), registered(false), isOpe(false), recvBuffer(""), _awayMessage("") {
    std::cout << "Client created" << std::endl;
}

Client::~Client() {
    std::cout << "Client destroyed." << std::endl;
}

// Setters

void Client::setNickname(const std::string &nickname) {
    Client::nickname = nickname;
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

void Client::setPassword(const std::string& password) {
    Client::password = password;
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

