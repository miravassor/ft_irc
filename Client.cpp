#include "Client.hpp"

// Build

Client::Client() : logged(false), registered(false), isOpe(false) {
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

