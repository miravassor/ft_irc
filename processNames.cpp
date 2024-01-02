#include "Server.hpp"

void Server::processNames(int fd, const std::vector<std::string> &tokens) {
	std::map<std::string, std::set<int> > channelsFds;
	if (tokens.size() == 1) {
		std::pair<std::string, std::set<int> > fdsWithoutChannels = std::make_pair("", getClientsFds());
		fillChannelsFds(channelsFds, &fdsWithoutChannels, _channels);
		channelsFds.insert(fdsWithoutChannels);
	} else {
		std::queue<std::string> channelNames = split(tokens[1], ',', true);
		std::vector<Channel *> channels = findChannels(channelNames);
		fillChannelsFds(channelsFds, NULL, channels);
	}
	for (std::map<std::string, std::set<int> >::iterator it = channelsFds.begin(); it != channelsFds.end(); ++it) {
		std::string nicknamesString = mergeTokensToString(getNicknames((*it).second), false);
		serverSendReply(fd, (*it).first, RPL_NAMREPLY, nicknamesString);
	}
	serverSendReply(fd, "", RPL_ENDOFNAMES, "");
}

void Server::fillChannelsFds(std::map<std::string, std::set<int> > &channelsFds,
							 std::pair<std::string, std::set<int> > *fdsWithoutChannels,
							 std::vector<Channel *> &channels) const {
	for (std::vector<Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		channelsFds.insert(std::make_pair((*it)->getName(), (*it)->getMemberFds()));

		if (fdsWithoutChannels) {
			for (std::set<int>::const_iterator fdIt = (*it)->getMemberFds().begin();
				 fdIt != (*it)->getMemberFds().end(); ++fdIt) {
				fdsWithoutChannels->second.erase(*fdIt);
			}
		}
	}
}