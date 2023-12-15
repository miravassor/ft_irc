#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>

class Channel {

private:
    std::string name;
    std::string topic;
    std::string mode;

public:

    Channel(const std::string &name);

    ~Channel();

};


#endif
