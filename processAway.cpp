#include "Server.hpp"

// User should not be able to use MODE +a command but use away command

void    Server::processAway(int fd, const std::vector<std::string> &tokens) {
    if (!clients[fd]->activeMode(AWAY) && tokens.size() < 2) {
        serverSendError(fd, "AWAY", ERR_NEEDMOREPARAMS);
        return;
    }
    if (!clients[fd]->activeMode(AWAY)) {
        clients[fd]->setAwayMessage(tokens[1]);
        clients[fd]->addMode(AWAY);
        serverSendReply(fd, clients[fd]->getNickname(), RPL_NOWAWAY, "");
    }
    else {
        clients[fd]->removeMode(AWAY);
        serverSendReply(fd, clients[fd]->getNickname(), RPL_UNAWAY, "");
    }
}