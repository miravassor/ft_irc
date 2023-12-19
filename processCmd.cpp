#include "Server.hpp"
#include "Client.hpp"

// implementation of commands

void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processJoin(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processInvite(int fd, const std::vector<std::string> &tokens) {
	(void)fd;
	(void) tokens;
}

void Server::processKick(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processPart(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

// process MODE command (user)
void Server::processMode(int fd, const std::vector<std::string> &tokens) {
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());
	if (params[0] != clients[fd]->getNickname()) {
		serverReply(fd, params[0], ERR_USERSDONTMATCH);
		return;
	}
	std::vector<std::string>::const_iterator it = tokens.begin() + 2;
	for (; it != tokens.end(); ++it) {
		Mode mode = clients[fd]->getMode(*it);
		if (mode == UNKNOWN) {
			serverReply(fd, *it, ERR_UMODEUNKNOWNFLAG);
			return;
		}
		else if (it[0][0] == '+') {
			clients[fd]->addMode(mode);
			serverReply(fd, *it, RPL_UMODEIS);
		}
		else if (it[0][0] == '-') {
			clients[fd]->removeMode(mode);
			serverReply(fd, *it, RPL_UMODEIS);
		}
	}
}

void Server::processPing(int fd, const std::vector<std::string> &tokens) {
	(void)tokens;
	serverReply(fd, "", PONG);
}
