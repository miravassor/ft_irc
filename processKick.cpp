#include "Server.hpp"

void Server::processKick(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3) {
		serverReply(fd, "KICK", ERR_NEEDMOREPARAMS);
		return;
	}

	const std::string &channelName = tokens[1];
	const std::string &targetNick = tokens[2];
	std::string reason = (tokens.size() > 3) ? " " + tokens[3] : "";
	Channel *channel = findChannel(channelName);
	if (!channel) {
		serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
	} else if (!channel->hasMember(fd)) {
		serverReply(fd, targetNick, ERR_NOTONCHANNEL);
	} else if (!channel->hasOperator(fd)) {
		serverReply(fd, channelName, ERR_CHANOPRIVSNEEDED);
	} else {
		Client *targetClient = findClient(targetNick);
		if (!targetClient || !channel->hasMember(targetClient->getSocket())) {
			serverReply(fd, targetNick + " " + channelName, ERR_USERNOTINCHANNEL);
		} else {
			std::string parameters = targetNick + " from " + channelName + reason;
			serverSendNotification(channel->getMemberFds(), getNick(fd), "KICK", parameters);
			channel->removeMember(targetClient->getSocket());
		}
	}
}