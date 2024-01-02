#include "Client.hpp"

// TODO : Rework to make a binary field instead of a set
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
		case 'r':
			return RESTRICTED;
		case 'o':
			return OPERATOR;
		default:
			return UNKNOWN;
	}
}

void	Client::addMode(Mode mode) {
    if (activeMode(mode))
        return;
    modes |= mode;
}

void	Client::removeMode(Mode mode) {
    if (!activeMode(mode))
        return;
    modes &= ~mode;
}

bool	Client::activeMode(Mode mode) const {
	return (modes & mode) == mode;
}
