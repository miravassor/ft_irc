#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// draft todo: refactoring needed
void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() == 1) {
		serverReply(fd, "", ERR_NORECIPIENT);
	} else if (tokens.size() == 2) {
		serverReply(fd, "", ERR_NOTEXTTOSEND);
	} else {
		std::queue<std::string> targets = split(tokens[1], ',');
		const std::string &message = tokens[2];
		std::string prefix = getNick(fd);
		std::set<std::string> uniqueTargets;

		while (!targets.empty()) {
			const std::string &targetName = targets.front();
			// if targetName is not double of one of previous names
			if (uniqueTargets.insert(targetName).second) {
				if (targetName.at(0) == '#' || targetName.at(0) == '&') { // for channel
					std::vector<Channel *>::iterator channelIt = findChannelIterator(targetName);
					if (channelIt == _channels.end()) {
						serverReply(fd, targetName, ERR_NOSUCHNICK);
					} else if (!(*channelIt)->hasMember(fd)) {
						serverReply(fd, targetName, ERR_CANNOTSENDTOCHAN);
					} else {
						std::string parameters = (*channelIt)->getName() + " " + message;
						serverSendNotification((*channelIt)->getMemberFds(), prefix, "PRIVMSG", parameters);
					}
				} else { // for user
					Client *receiver = findClient(targetName);
					if (!receiver) {
						serverReply(fd, targetName, ERR_NOSUCHNICK);
					} else {
						std::string parameters = targetName + " " + message;
						serverSendNotification(receiver->getSocket(), prefix, "PRIVMSG", parameters);
						if (receiver->activeMode(AWAY)) {
							serverSendReply(fd, "301", targetName, receiver->getAwayMessage()); // RPL_AWAY
						}
					}
				}
			}
			targets.pop();
		}
	}
}

// that method will be refactored later
void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());

	if (tokens.size() < 2)
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
	// search if channel already exist
	// check if user already in channel ??
	std::vector<Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++it) {
		if ((*it)->getName() == tokens[1]) {
			(*it)->addMember(fd);
			(*it)->newMember(fd);
			return;
		}
	}
	// or build one
	if (tokens[1].find(' ') != std::string::npos) {
		// no space allowed in channel name
		serverReply(fd, tokens[1], ERR_NOSUCHCHANNEL);
		return;
	} else {
		_channels.push_back(new Channel(tokens[1], this));
		_channels.back()->addMember(fd);
		_channels.back()->addOperator(fd);
		_channels.back()->newMember(fd);
	}
}

// void Server::processJoin(int fd, const std::vector<std::string> &tokens) {
// 	std::queue<std::string> channels = split(tokens[1], ',');
// 	std::queue<std::string> passwords;
// 	if (tokens.size() > 2) {
// 		passwords = split(tokens[2], ',');
// 	}
// 	for (; !channels.empty(); channels.pop()) {
// 		std::string channelName = channels.front();
// 		std::string password = passwords.empty() ? "" : passwords.front();
// 		Channel *channel = findChannel(channelName);
// 		//todo: refactoring
// 		if (channel) {
// 			if (channel->authMember(fd, password)) {
// 				// serverReply(fd, *it, RPL_JOIN);
// 				// notifyUsersOfChannel()..
// 			} else {
// 				serverReply(fd, channelName, ERR_BADCHANNELKEY);
// 			}
// 		} else {
// 			if (isValidChannelName(channelName)) {
// 				Channel *newChannel = new Channel(channelName, password);
// 				newChannel->addMember(fd);
// 				newChannel->addOperator(fd);
// 				addChannel(newChannel);
// 				// serverReply(fd, *it, RPL_JOIN);
// 				// notifyUsersOfChannel()..
// 			} else {
// 				serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
// 			}
// 		}
// 	}
// }

// that method can be moved lately into some utils file
std::queue<std::string> Server::split(const std::string &src, char delimiter) const {
	std::queue<std::string> tokens;
	std::istringstream srcStream(src);
	std::string token;
	while (std::getline(srcStream, token, delimiter)) {
		tokens.push(token);
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
		} else if (!channel->isModeSet(ALLTOPICSET) && !channel->hasOperator(fd)) {
			serverReply(fd, channelName, ERR_CHANOPRIVSNEEDED);
		} else {
			const std::string &newTopic = tokens[2];
			channel->setTopic(newTopic);
			serverSendNotification(channel->getMemberFds(), getNick(fd), "TOPIC", channelName + " :" + newTopic);
		}
	}
}

void Server::processPart(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverReply(fd, "PART", ERR_NEEDMOREPARAMS);
		return;
	}

	std::queue<std::string> channels = split(tokens[1], ',');
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

void Server::processMode(int fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		serverReply(fd, "MODE", ERR_NEEDMOREPARAMS);
		return;
	}

	if (tokens[1].at(0) == '#' || tokens[1].at(0) == '&') {
		processChannelMode(fd, tokens);
	} else {
		processUserMode(fd, tokens);
	}
}

// in progress
void Server::processChannelMode(int fd, const std::vector<std::string> &tokens) {
	const std::string& channelName = tokens[1];
	Channel *channel = findChannel(channelName);
	if (!channel) {
		serverReply(fd, channelName, ERR_NOSUCHCHANNEL);
	} else if (tokens.size() == 2) {
		// display channel modes
		serverSendReply(fd, "324", getNick(fd) + " " + channelName, channel->getModeString());
	} else {

		// mode exists
		// operator rights
		// change mode or do nothing or reply with error
		// notify clients
	}
}

// process MODE command (user) !!-> doc has more
void Server::processUserMode(int fd, const std::vector<std::string> &tokens) {
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
	} else {
		std::string pong = ":42.IRC PONG " + tokens[1];
		serverReply(fd, pong, PONG);
	}
}
