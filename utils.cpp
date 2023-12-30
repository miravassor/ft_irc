#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"


// probably better to have capitalized names/nicknames in channels/clients than capitalize each time when looking
std::string Server::capitalizeString(const std::string &input) {
	std::string output(input);
	for (size_t i = 0; i < input.size(); i++) {
		output[i] = toupper(input[i]);
	}
	return output;
}

std::queue<std::string> Server::split(const std::string &src, char delimiter, bool unique) const {
	std::set<std::string> uniqueTokens;
	std::queue<std::string> tokensQueue;
	std::istringstream srcStream(src);
	std::string token;

	while (std::getline(srcStream, token, delimiter)) {
		if (unique) {
			uniqueTokens.insert(token);
		} else {
			tokensQueue.push(token);
		}
	}
	if (unique) {
		for (std::set<std::string>::const_iterator it = uniqueTokens.begin(); it != uniqueTokens.end(); ++it) {
			tokensQueue.push(*it);
		}
	}
	return tokensQueue;
}

bool Server::isValidChannelName(const std::string &name) {
	if (name[0] != '#' && name[0] != '&') {
		return false;
	}
	return isValidName(name.substr(1));
}

bool Server::isValidName(const std::string &name) {
	if (name.empty()) {
		return false;
	}
	for (size_t i = 0; i < name.size(); i++) {
		if (name[i] == ' ' || name[i] == 0 || name[i] == 7 || name[i] == 13 || name[i] == 10) {
			return false;
		}
	}
	return true;
}

std::string Server::mergeTokensToString(const std::vector<std::string> &tokens) {
	std::string mergedString;
	for (size_t i = 0; i < tokens.size(); ++i) {
		mergedString += tokens[i];
		if (i < tokens.size() - 1) {
			mergedString += " ";
		}
	}
	return mergedString;
}