#include "Server.hpp"

void Server::processPart(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverSendError(fd, "PART", ERR_NEEDMOREPARAMS);
		return;
	}

	std::queue<std::string> channels = split(tokens[1], ',', true);
	if (channels.size() > MAXTARGETS) {
		serverSendError(fd, "PART", ERR_TOOMANYTARGETS);
		return;
	}
	std::string reason;
	if (tokens.size() > 2) {
		reason = tokens[2].at(0) == ':'
				 ? mergeTokensToString(std::vector<std::string>(tokens.begin() + 2, tokens.end()), true)
				 : tokens[2];
	}
	while (!channels.empty()) {
		std::string channelName = channels.front();
		std::vector<Channel *>::iterator channelIt = findChannelIterator(channelName);
		if (channelIt == _channels.end()) {
			serverSendError(fd, channelName, ERR_NOSUCHCHANNEL);
		} else if (!(*channelIt)->hasMember(fd)) {
			serverSendError(fd, channelName, ERR_NOTONCHANNEL);
		} else {
			serverSendNotification((*channelIt)->getMemberFds(), getNick(fd), "PART", channelName + " :" + reason);
			(*channelIt)->removeMember(fd);
			clients[fd]->removeChannel(channelName);
			if ((*channelIt)->getMemberFds().empty()) {
				delete *channelIt;
				_channels.erase(channelIt);
			}
		}
		channels.pop();
	}
}