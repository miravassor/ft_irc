#include "Server.hpp"

void    Server  ::processWho(int fd, const std::vector<std::string> &tokens) {
    if (tokens.size() > 1 && tokens[1][0] == '#') {
        Channel *channel = findChannel(tokens[1]);
        if (channel) {
            std::set<int>::iterator members = channel->getMemberFds().begin();
            for (; members != channel->getMemberFds().end(); ++members) {
                if (clients[*members] != clients[fd] && !clients[*members]->activeMode(INVISIBLE)) {
                    std::string token = channel->getName() + " " + clients[*members]->getNickname() + " ";
                    token.append(clients[fd]->getNickname() + +" localhost " + serverName + " ");
                    token.append(clients[*members]->getNickname() + " ");
                    if (clients[*members]->activeMode(AWAY)) {
                        token.append("G");
                    } else {
                        token.append("H");
                    }
                    if (channel->isOperator(*members)) {
                        token.append("@");
                    }
                    std::string result = "0 ";
                    result.append(clients[*members]->getRealName());
                    serverSendReply(fd, token, RPL_WHOREPLY, result);
                }
            }
        }
        serverSendReply(fd, channel->getName(), RPL_ENDOFWHO, "");
    }
}

void    Server::listServerClients(int fd) {
    std::map<int, Client *>::const_iterator it = clients.begin();
    for (; it != clients.end(); ++it) {
            if (clients[fd] != it->second && !clients[it->second->getSocket()]->activeMode(INVISIBLE)) {
                std::string token = clients[it->second->getSocket()]->getNickname() + " ";
                token.append(clients[fd]->getNickname() + + " localhost " + serverName + " ");
                token.append(clients[it->second->getSocket()]->getNickname() + " ");
                if (clients[it->second->getSocket()]->activeMode(AWAY)) {
                    token.append("G");
                }
                else {
                    token.append("H");
                }
                std::string result = "0 ";
                result.append(clients[it->second->getSocket()]->getRealName());
                serverSendReply(fd, token, RPL_WHOREPLY, result);
            }
    }
}

void    Server::processWhois(int fd, const std::vector<std::string> &tokens) {
    (void)tokens;
    serverSendReply(fd, "", RPL_ENDOFWHOIS, "");
}