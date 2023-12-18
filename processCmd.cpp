#include "Server.hpp"

// implementation of commands

void Server::processPrivmsg(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processJoin(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processInvite(int fd, const std::vector<std::string> &tokens) {
	(void)fd;
	(void) tokens;
}

void Server::processKick(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}

void Server::processPart(int fd, const std::vector<std::string> &tokens){
	(void)fd;
	(void) tokens;
}
