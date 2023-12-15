#include "Client.hpp"

Client::Client() {
    std::cout << "Client created" << std::endl;
}

Client::~Client() {
    std::cout << "Client destroyed." << std::endl;
}

const std::string &Client::getNickname() const {
    return nickname;
}

void Client::setNickname(const std::string &nickname) {
    Client::nickname = nickname;
}
