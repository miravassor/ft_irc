#include "Server.hpp"

void Server::processPart(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverReply(fd, "PART", ERR_NEEDMOREPARAMS);
		return;
	}

	std::queue<std::string> channels = split(tokens[1], ',', false);
	std::string reason = tokens.size() > 2 ? (" " + tokens[2]) : "";
	std::string prefix = getNick(fd);

	while (!channels.empty()) {
		std::string channelName = channels.front();
		std::vector<Channel *>::iterator channelIt = findChannelIterator(channelName);
		if (channelIt == _channels.end()) {
			serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
		} else if (!(*channelIt)->hasMember(fd)) {
			serverReply(fd, channelName, ERR_NOTONCHANNEL);
		} else {
			std::string parameters = (*channelIt)->getName() + reason;
			serverSendNotification((*channelIt)->getMemberFds(), prefix, "PART", parameters);
			(*channelIt)->removeMember(fd);
			if ((*channelIt)->getMemberFds().empty()) {
				delete *channelIt;
				_channels.erase(channelIt);
			}
		}
		channels.pop();
	}
}