#include "Server.hpp"

void Server::processInvite(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3) {
		serverReply(fd, "INVITE", ERR_NEEDMOREPARAMS);
		return;
	}

	const std::string &invitedNick = tokens[1];
	const std::string &channelName = tokens[2];
	Client *invitedClient = findClient(invitedNick);
	Channel *channel = findChannel(channelName);
	std::string parameters = invitedNick + " " + channelName;
	if (!invitedClient) {
		serverReply(fd, invitedNick, ERR_NOSUCHNICK);
	} else if (!channel) {
		serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
	} else if (!channel->hasMember(fd)) {
		serverReply(fd, channelName, ERR_NOTONCHANNEL);
	} else if (channel->hasMember(invitedClient->getSocket())) {
		serverReply(fd, parameters, ERR_USERONCHANNEL);
	} else {
		if (channel->isModeSet(INVITEONLY)) {
			if (!channel->hasOperator(fd)) {
				serverReply(fd, channelName, ERR_CHANOPRIVSNEEDED);
				return;
			}
			channel->addInvited(invitedClient->getSocket());
		}
		const std::string &inviterNick = getNick(fd);
		serverSendNotification(invitedClient->getSocket(), inviterNick, "INVITE", parameters);
		serverSendReply(fd, "341", inviterNick, parameters); // RPL_INVITING
		if (invitedClient->activeMode(AWAY)) {
			serverSendReply(fd, "301", invitedNick, invitedClient->getAwayMessage()); // RPL_AWAY
		}
	}
}