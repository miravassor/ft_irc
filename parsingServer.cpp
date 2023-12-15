#include "Server.hpp"

void	Server::registrationProcess(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
			return;

	std::string command = tokens[0];
	if (command == "CAP") {
		if (tokens[1] == "LS") {
			std::string	capls = "Entering registration process\r\n";
			send(fd, capls.c_str(), capls.length(), 0);
		}
	}
	if (command == "PASS") {
		if (tokens[1] == password)
			clients[fd]->setLog();
		else {
			;
			// 	removeClient(clients[fd]->getNickname());
			// 	pollFds.erase(pollFds.begin() + 1);
		}
	}
	if (command == "NICK") {
		clients[fd]->setNickname(tokens[1]);
	}
	if (command == "USER") {
		clients[fd]->setUsername(tokens[1]);
	}
	checkRegistration(fd);
}

void	Server::checkRegistration(int fd) {
	Client* client = clients[fd];

	if (client->isLogged()) {
		if (!client->getNickname().empty() && !client->getUsername().empty()) {
			client->setRegistration();
			completeRegistration(fd, client);
		}
	}
}

void	Server::completeRegistration(int fd, Client *client) {
	std::string welcome = ":" + serverName + " 001 " + client->getNickname() + " :Welcome to the IRC Network, " + client->getNickname() + "\r\n";
	std::string host = ":" + serverName + " 002 " + client->getNickname() + " :Your host is " + serverName + ", running version 0.1" + "\r\n";
	std::string created = ":" + serverName + " 003 " + client->getNickname() + " :This server was created " +  ctime(&start) + "\r\n";
	std::string myinfo  = ":" + serverName + " 004 " + client->getNickname() + " :INFOS [...]" + "\r\n";
	std::string support  = ":" + serverName + " 005 " + client->getNickname() + " :are supported by this server [...]" + "\r\n";
	send(fd, welcome.c_str(), welcome.length(), 0);
	send(fd, host.c_str(), host.length(), 0);
	send(fd, created.c_str(), created.length(), 0);
	send(fd, myinfo.c_str(), myinfo.length(), 0);
	send(fd, support.c_str(), support.length(), 0);
}

void	Server::parsBuffer(int fd) {
	std::string					bufferStr(_buffer);
	std::stringstream			ss(bufferStr);
	std::string					line;

	// tokenize the buffer line by line
	while (std::getline(ss, line))
	{
		size_t	cRet = line.find('\r');
		if (cRet != std::string::npos)
			line.erase(cRet);

		std::istringstream lineStream(line);
		std::vector<std::string> tokens;
		std::string token;
		while (lineStream >> token) {
			std::cout << token << " ";
			tokens.push_back(token);
		}
		std::cout << "[TOKEN END]" << std::endl;

		if (clients[fd]->isRegistered() == false)
			registrationProcess(fd, tokens);
		else
			processCmd(fd, tokens);
	}
}

void	Server::processCmd(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
		return;

	std::string command = tokens[0];
	(void)fd;
}
