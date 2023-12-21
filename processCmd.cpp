#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// implementation of commands

void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processJoin(int fd, const std::vector<std::string> &tokens){
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());

	if (tokens.size() < 2)
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
	// search if channel already exist
	// check if user already in channel ??
	std::vector<Channel *>::iterator it = channels.begin();
	for (; it != channels.end(); ++it) {
		if ((*it)->getName() == tokens[1]) {
			(*it)->addMember(fd);
			(*it)->newMember(fd);
			return;
		}
	}
	// or build one
	if (tokens[1].find(' ') != std::string::npos) {
		// no space allowed in channel name
		serverReply(fd, tokens[1], ERR_NOSUCHCHANNEL);
		return;
	}
	else {
		channels.push_back(new Channel(tokens[1], this));
		channels.back()->addMember(fd);
		channels.back()->addOperator(fd);
		channels.back()->newMember(fd);
	}
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

// process MODE command (user) !!-> doc has more
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
	if (tokens.size() <= 1) {
		serverReply(fd, "", ERR_NOORIGIN);
	}
	else if (tokens[1] != serverName) {
		serverReply(fd, "", ERR_NOSUCHSERVER);
	}
	else {
		std::string pong = ":42.IRC PONG " + tokens[1];
		serverReply(fd, pong, PONG);
	}
}
