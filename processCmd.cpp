#include "Server.hpp"
#include "Client.hpp"

// implementation of commands

void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens) {
	(void) fd;
	(void) tokens;
}

// todo: private channels, passwords for channels
void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
	std::vector<std::string> channels = split(tokens[1], ',');
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
		std::cout << "Client with fd=" << fd << " requested to join channel " << *it << std::endl;
		Channel *channel = findChannel(*it);
		if (channel) {
			channel->addMember(fd);
			// serverReply(fd, *it, RPL_JOIN);
			// notifyUsersOfChannel()..
		} else {
			if (isValidChannelName(*it)) {
				Channel *newChannel = new Channel(*it);
				newChannel->addMember(fd);
				newChannel->addOperator(fd);
				addChannel(newChannel);
				// serverReply(fd, *it, RPL_JOIN);
				// notifyUsersOfChannel()..
			} else {
				serverReply(fd, *it, ERR_NOSUCHCHANNEL);
			}
		}
	}
}

// that method can be moved lately into some utils file
std::vector<std::string> Server::split(const std::string &src, char delimiter) const {
	std::vector<std::string> tokens;
	std::istringstream channelStream(src);
	std::string channel;
	while (std::getline(channelStream, channel, delimiter)) {
		tokens.push_back(channel);
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
	} else
		serverReply(fd, "", PONG);
}
