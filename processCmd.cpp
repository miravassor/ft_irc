#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// implementation of commands

void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens) {
	(void) fd;
	(void) tokens;
}

void Server::processJoin(int fd, const std::vector<std::string> &tokens){
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());

	if (tokens.size() < 2)
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
	// search if channel already exist
	// check if user already in channel ??
	std::vector<Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++it) {
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
		_channels.push_back(new Channel(tokens[1], this));
		_channels.back()->addMember(fd);
		_channels.back()->addOperator(fd);
		_channels.back()->newMember(fd);
	}
}

// void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
// 	std::queue<std::string> channels = split(tokens[1], ',');
// 	std::queue<std::string> passwords;
// 	if (tokens.size() > 2) {
// 		passwords = split(tokens[2], ',');
// 	}
// 	for (; !channels.empty(); channels.pop()) {
// 		std::string channelName = channels.front();
// 		std::string password = passwords.empty() ? "" : passwords.front();
// 		Channel *channel = findChannel(channelName);
// 		//todo: refactoring
// 		if (channel) {
// 			if (channel->authMember(fd, password)) {
// 				// serverReply(fd, *it, RPL_JOIN);
// 				// notifyUsersOfChannel()..
// 			} else {
// 				serverReply(fd, channelName, ERR_BADCHANNELKEY);
// 			}
// 		} else {
// 			if (isValidChannelName(channelName)) {
// 				Channel *newChannel = new Channel(channelName, password);
// 				newChannel->addMember(fd);
// 				newChannel->addOperator(fd);
// 				addChannel(newChannel);
// 				// serverReply(fd, *it, RPL_JOIN);
// 				// notifyUsersOfChannel()..
// 			} else {
// 				serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
// 			}
// 		}
// 	}
// }

// that method can be moved lately into some utils file
std::queue<std::string> Server::split(const std::string &src, char delimiter) const {
	std::queue<std::string> tokens;
	std::istringstream srcStream(src);
	std::string token;
	while (std::getline(srcStream, token, delimiter)) {
		tokens.push(token);
	}
	return tokens;
}

bool Server::isValidChannelName(const std::string &name) {
	if (name[0] != '#' && name[0] != '&') {
		return false;
	}
	for (size_t i = 1; i < name.size(); i++) {
		if (name[i] == ' ' || name[i] == 0 || name[i] == 7 || name[i] == 13 || name[i] == 10) {
			return false;
		}
	}
	return true;
}

void Server::processInvite(int fd, const std::vector<std::string> &tokens) {
	(void) fd;
	(void) tokens;
}

void Server::processKick(int fd, const std::vector<std::string> &tokens) {
	(void) fd;
	(void) tokens;
}

void Server::processPart(int fd, const std::vector<std::string> &tokens) {
	(void) fd;
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
		} else if (it[0][0] == '+') {
			clients[fd]->addMode(mode);
			serverReply(fd, *it, RPL_UMODEIS);
		} else if (it[0][0] == '-') {
			clients[fd]->removeMode(mode);
			serverReply(fd, *it, RPL_UMODEIS);
		}
	}
}

void Server::processPing(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() <= 1) {
		serverReply(fd, "", ERR_NOORIGIN);
	} else if (tokens[1] != serverName) {
		serverReply(fd, "", ERR_NOSUCHSERVER);
	}
	else {
		std::string pong = ":42.IRC PONG " + tokens[1];
		serverReply(fd, pong, PONG);
	}
}
