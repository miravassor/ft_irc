#include "Server.hpp"

void Server::processNames(int fd, const std::vector<std::string> &tokens) {
	std::map<std::string, std::vector<std::string> > nicks;
	if (tokens.size() == 1) {
		nicks = getClientsOfChannels(fd, _channels);
		std::pair<std::string, std::vector<std::string> > othersNicks = std::make_pair("", getClientsWithoutChannels());
		nicks.insert(othersNicks);
	} else {
		std::queue<std::string> channelNames = split(tokens[1], ',', true);
		std::vector<Channel *> channels = findChannels(channelNames);
		nicks = getClientsOfChannels(fd, channels);
	}
	for (std::map<std::string, std::vector<std::string> >::iterator it = nicks.begin(); it != nicks.end(); ++it) {
		std::string nicknamesString = mergeTokensToString(it->second, false);
		serverSendReply(fd, (*it).first, RPL_NAMREPLY, nicknamesString);
	}
	serverSendReply(fd, "", RPL_ENDOFNAMES, "");
}

std::map<std::string, std::vector<std::string> > Server::getClientsOfChannels(int fd, std::vector<Channel *> channels) {
	std::map<std::string, std::vector<std::string> > nicks;
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
		std::vector<std::string> channelNicks;
		Channel *channel = *it;
		if (channel->hasMember(fd)) {
			channelNicks = getAllChannelMembersNicks(channel);
		} else {
			channelNicks = getVisibleChannelMembersNicks(channel);
		}
		nicks.insert(std::make_pair(channel->getName(), channelNicks));
	}
	return nicks;
}
std::vector<std::string> Server::getAllChannelMembersNicks(const Channel *channel) {
	std::vector<std::string> nicks;
	std::set<int> members = channel->getMemberFds();
	for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it) {
		std::string nick = clients[*it]->getNickname();
		if (channel->hasOperator(*it)) {
			nick = "@" + nick;
		}
		nicks.push_back(nick);
	}
	return nicks;
}

std::vector<std::string> Server::getVisibleChannelMembersNicks(const Channel *channel) {
	std::vector<std::string> nicks;
	std::set<int> members = channel->getMemberFds();
	for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it) {
		if (!clients[*it]->activeMode(INVISIBLE)) {
			std::string nick = clients[*it]->getNickname();
			if (channel->hasOperator(*it)) {
				nick = "@" + nick;
			}
			nicks.push_back(nick);
		}
	}
	return nicks;
}

std::vector<std::string> Server::getClientsWithoutChannels() {
	std::vector<std::string> nicks;
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
		Client *client = it->second;
		if (!client->activeMode(INVISIBLE) && client->getChannels().empty()) {
			nicks.push_back(client->getNickname());
		}
	}
	return nicks;
}