#include "Server.hpp"

void Server::processList(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() == 1) {
		listChannels(fd, _channels);
	} else {
		std::queue<std::string> channelNames = split(tokens[1], ',', true);
		std::vector<Channel *> channels = findChannels(channelNames);
		listChannels(fd, channels);
	}
	serverSendReply(fd, "", RPL_LISTEND, "");
}

void Server::listChannels(int fd, std::vector<Channel *> &channels) {
	for (std::vector<Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		std::stringstream clientCount;
		clientCount << (*it)->getMemberFds().size();
		serverSendReply(fd,
						(*it)->getName() + " " + clientCount.str(),
						RPL_LIST,
						!(*it)->getTopic().empty() ? (*it)->getTopic() : "");
	}
}