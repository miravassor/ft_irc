#include "Client.hpp"

Mode	Client::getMode(const std::string &mode) {
	if (mode.size() != 2)
		return UNKNOWN;
	if (mode[0] != '+' && mode[0] != '-')
		return UNKNOWN;
	switch (mode[1]) {
		case 'a' :
			return AWAY;
		case 'i':
			return INVISIBLE;
		case 'w':
			return WALLOPS;
		case 'r':
			return RESTRICTED;
		case 'o':
			return OPERATOR;
		case 'O':
			return LOCAL_OPERATOR;
		case 's':
			return SERVER_NOTICES;
		default:
			return UNKNOWN;
	}
}

void	Client::addMode(Mode mode) {
	modes.insert(mode);
}

void	Client::removeMode(Mode mode) {
	modes.erase(mode);
}

bool	Client::activeMode(Mode mode) const {
	return modes.find(mode) != modes.end();
}
