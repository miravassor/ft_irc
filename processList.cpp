#include "Server.hpp"

void Server::processList(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() == 1) {
		listChannels(fd, _channels);
	} else {
		std::queue<std::string> channelNames = split(tokens[1], ',', true);
		std::vector<Channel *> channels = findChannels(channelNames);
		listChannels(fd, channels);
	}
	serverReply(fd, "", RPL_LISTEND);
}

void Server::listChannels(int fd, std::vector<Channel *> &channels) {
	for (std::vector<Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		std::string topic;
		if (!(*it)->getTopic().empty()) {
			topic = (*it)->getTopic();
		}
		serverSendReply(fd, "322", (*it)->getName(), topic);
	}
}