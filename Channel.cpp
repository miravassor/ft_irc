#include "Channel.hpp"

Channel::Channel(const std::string &name) {
    this->name = name;
    std::cout << "Channel " << this->name << " created." << std::endl;
}

Channel::~Channel() {
    std::cout << "Channel " << this->name << " destroyed." << std::endl;
}
