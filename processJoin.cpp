#include "Server.hpp"

void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverReply(fd, "JOIN", ERR_NEEDMOREPARAMS);
		return;
	}

	std::queue<std::string> channels = split(tokens[1], ',', true);
	std::queue<std::string> passwords;
	if (tokens.size() > 2) {
		passwords = split(tokens[2], ',', false);
	}

	for (; !channels.empty(); channels.pop()) {
		std::string channelName = channels.front();
		std::string password = passwords.empty() ? "" : passwords.front();
		Channel *channel = findChannel(channelName);
		if (channel) { // join existing channel
			if (channel->isModeSet(INVITEONLY) && !channel->hasInvited(fd)) {
				serverReply(fd, channelName, ERR_INVITEONLYCHAN);
				return;
			}
			if (channel->isModeSet(LIMITSET)
			    && (int) channel->getMemberFds().size() == channel->getLimitMembers()) {
				serverReply(fd, channelName, ERR_CHANNELISFULL);
				return;
			}
			if (channel->authMember(fd, password)) { // checking password and removing from invited container
				sendJoinNotificationsAndReplies(fd, channel);
			} else {
				serverReply(fd, channelName, ERR_BADCHANNELKEY);
			}
		} else { // creating new channel
			if (isValidChannelName(channelName)) {
				Channel *newChannel = new Channel(channelName, password);
				newChannel->addMember(fd);
				newChannel->addOperator(fd);
				addChannel(newChannel);
				sendJoinNotificationsAndReplies(fd, newChannel);
			} else {
				serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
			}
		}
		if (!passwords.empty()) {
			passwords.pop();
		}
	}
}

void Server::sendJoinNotificationsAndReplies(int fd, const Channel *channel) {
	serverSendNotification(channel->getMemberFds(), getNick(fd), "JOIN", channel->getName());
	if (!channel->getTopic().empty()) {
		serverSendReply(fd, "332", channel->getName(), channel->getTopic()); // RPL_TOPIC
	}
	std::string nicknamesString = mergeTokensToString(getNicknames(channel->getMemberFds()));
	serverSendReply(fd, "353", channel->getName(), nicknamesString); // RPL_NAMREPLY
	serverReply(fd, channel->getName(), RPL_ENDOFNAMES);
}

// that method will be refactored later
//void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
//	std::vector<std::string> params(tokens.begin() + 1, tokens.end());
//
//	if (tokens.size() < 2)
//		serverReply(fd, "", ERR_NEEDMOREPARAMS);
//	// search if channel already exist
//	// check if user already in channel ??
//	std::vector<Channel *>::iterator it = _channels.begin();
//	for (; it != _channels.end(); ++it) {
//		if ((*it)->getName() == capitalizeString(tokens[1])) {
//			(*it)->addMember(fd);
//			(*it)->newMember(fd);
//			return;
//		}
//	}
//	// or build one
//	if (tokens[1].find(' ') != std::string::npos) {
//		// no space allowed in channel name
//		serverReply(fd, tokens[1], ERR_NOSUCHCHANNEL);
//		return;
//	} else {
//		_channels.push_back(new Channel(capitalizeString(tokens[1]), this));
//		_channels.back()->addMember(fd);
//		_channels.back()->addOperator(fd);
//		_channels.back()->newMember(fd);
//	}
//}