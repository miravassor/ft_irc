#include "Server.hpp"

void Server::processQuit(int fd, const std::vector<std::string> &tokens) {
	std::vector<std::string> channels = getClient(fd).getChannels();
	std::set<int> sharingChannelsFds;
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel *channel = findChannel(*it);
		if (channel) {
			const std::set<int> &memberFds = channel->getMemberFds();
			sharingChannelsFds.insert(memberFds.begin(), memberFds.end());
		}
	}
	sharingChannelsFds.erase(fd);
	std::string reason;
	if (tokens.empty()) {
		reason = "Remote host closed connection";
	} else {
		reason = "Client quit";
		if (tokens.size() > 1) {
			reason = tokens[1].at(0) == ':'
					 ? mergeTokensToString(std::vector<std::string>(tokens.begin() + 1, tokens.end()), true)
					 : tokens[1];
		}
		std::string notification = ":" + getNick(fd) + " QUIT :" + reason + "\r\n";
		std::string err = "ERROR :Closing connection\r\n";
		send(fd, notification.c_str(), notification.length(), 0);
		send(fd, err.c_str(), err.length(), 0);
		close(fd);
	}
	serverSendNotification(sharingChannelsFds, getNick(fd), "QUIT", ":" + reason);
	removeClient(fd);
}