#include "Server.hpp"

void    Server  ::processWho(int fd, const std::vector<std::string> &tokens) {
    if (tokens.size() > 1 && tokens[1][0] == '#') {
        Channel *channel = findChannel(tokens[1]);
        if (channel) {
            std::set<int>::iterator members = channel->getMemberFds().begin();
            for (; members != channel->getMemberFds().end(); ++members) {
                std::string result = channel->getName()  + " " + serverName + " " +  clients[fd]->getNickname() + " ";
                if (clients[*members] != clients[fd] && !clients[*members]->activeMode(INVISIBLE)) {
                    if (clients[*members]->activeMode(AWAY))
                        result.append("G");
                    else
                        result.append("H");
                    if (channel->isOperator(*members))
                        result.append("@");
                    result.append(" :");
                    result.append(clients[*members]->getRealName());
                    serverSendReply(fd, clients[fd]->getNickname(), RPL_WHOREPLY, result);
                }
            }
        }
    }
//    else if (tokens.size() == 1) {
//        std::map<int, Client *>::iterator  it = clients.begin();
//        for (; it != clients.end(); ++it) {
//            if (clients[fd]->isInChannel(*cIt->))
//        }
//    }
    serverSendReply(fd, "", RPL_ENDOFWHO, "");
}

void    Server::processWhois(int fd, const std::vector<std::string> &tokens) {
    (void)tokens;
    serverSendReply(fd, "", RPL_ENDOFWHOIS, "");
}