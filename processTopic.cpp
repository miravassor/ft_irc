#include "Server.hpp"

void Server::processTopic(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverReply(fd, "TOPIC", ERR_NEEDMOREPARAMS);
		return;
	}

	const std::string &channelName = tokens[1];
	Channel *channel = findChannel(channelName);
	if (!channel) {
		serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
	} else if (tokens.size() == 2) {
		std::string topic = channel->getTopic();
		if (topic.empty()) {
			serverSendReply(fd, "331", channelName, "No topic is set"); // RPL_NOTOPIC
		} else {
			serverSendReply(fd, "332", channelName, channel->getTopic()); // RPL_TOPIC
		}
	} else {
		if (!channel->hasMember(fd)) {
			serverReply(fd, channelName, ERR_NOTONCHANNEL);
		} else if (channel->isModeSet(TOPICSET) && !channel->hasOperator(fd)) {
			serverReply(fd, channelName, ERR_CHANOPRIVSNEEDED);
		} else {
			const std::string &newTopic = tokens[2];
			channel->setTopic(newTopic);
			serverSendNotification(channel->getMemberFds(), getNick(fd), "TOPIC", channelName + " :" + newTopic);
		}
	}
}