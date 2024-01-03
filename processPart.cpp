#include "Server.hpp"

// todo: take the whole reason message after :
// in progress
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
	std::string reason = tokens.size() > 2 ? (" " + tokens[2]) : "";
	std::string prefix = getNick(fd);

	while (!channels.empty()) {
		std::string channelName = channels.front();
		std::vector<Channel *>::iterator channelIt = findChannelIterator(channelName);
		if (channelIt == _channels.end()) {
			serverSendError(fd, channelName, ERR_NOSUCHCHANNEL);
		} else if (!(*channelIt)->hasMember(fd)) {
			serverSendError(fd, channelName, ERR_NOTONCHANNEL);
		} else {
			std::string parameters = (*channelIt)->getName() + reason;
			serverSendNotification((*channelIt)->getMemberFds(), prefix, "PART", parameters);
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