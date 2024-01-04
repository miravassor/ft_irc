#include "Server.hpp"

void    Server  ::processWho(int fd, const std::vector<std::string> &tokens) {
    if (tokens.size() > 1 && tokens[1][0] == '#') {
        Channel *channel = findChannel(tokens[1]);
        if (channel) {
            std::set<int>::iterator members = channel->getMemberFds().begin();
            for (; members != channel->getMemberFds().end(); ++members) {
                std::string result = channel->getName()  + " " + serverName + " " +  clients[fd]->getNickname();
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
    serverSendReply(fd, "", RPL_ENDOFWHO, "");
}

//void Server::processPing(int fd, const std::vector<std::string> &tokens) {
//    if (tokens.size() <= 1) {
//        serverSendError(fd, "", ERR_NOORIGIN);
//    } else if (tokens[1] != serverName) {
//        serverSendError(fd, "", ERR_NOSUCHSERVER);
//    } else {
//        std::string pong = "PONG :42.IRC";
//        serverSendMessage(fd, pong);
//    }
//}