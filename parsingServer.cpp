#include "Server.hpp"

void	Server::registrationProcess(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
			return;

	std::string command = tokens[0];
	// if (command == "CAP") {
	// 	;
	// 	// if (tokens[1] == "LS") {
	// 		// std::string	welcomeMsg = "Welcome to our IRC server\r\n";
	// 		// send(4, welcomeMsg.c_str(), welcomeMsg.length(), 0);
	// 	// }
	// }
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
			std::string welcome = ":YourServer 001 " + client->getNickname() + " :Welcome to the IRC Network, " + client->getNickname() + "\r\n";
			send(fd, welcome.c_str(), welcome.length(), 0);
		}
	}
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
