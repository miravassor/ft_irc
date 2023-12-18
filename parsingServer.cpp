#include "Server.hpp"

// Parsing

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

		if (clients[fd]->isRegistered() == false) {
			if (registrationProcess(fd, tokens))
				return;
		}
		else
			processCmd(fd, tokens);
	}
}

bool	Server::registrationProcess(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
			return 0; // or 1?

	if (tokens.size() < 2) {
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	std::string command = tokens[0];
	std::vector<std::string>	params(tokens.begin() + 1, tokens.end());
	if (command == "CAP") {
		if (tokens[1] == "LS") {
			serverReply(fd, "", CAPLS);
		}
	}
	else if (handleCommand(fd, command, params)) {
		removeClient(clients[fd]->getSocket());
		return 1;
	}
	checkRegistration(fd);
	return 0;
}

bool	Server::handleCommand(int fd, const std::string& command, const std::vector<std::string>& arg) {
	if (command == "PASS") {
		return verifyPassword(fd, arg[0]);
	} else if (command == "NICK") {
		if (!verifyNickname(fd, arg[0]))
			return (clients[fd]->setNickname(arg[0]), 0);
		return 1;
	} else if (command == "USER") {
		std::string	realname = getParam(arg);
		if (!verifyUsername(fd, realname)) {
			return (clients[fd]->setUsername(realname), 0);
		}
		return 1;
	}
	return 1;
}

void Server::processCmd(int fd, std::vector<std::string>& tokens) {
    if (tokens.empty())
        return;

    std::string command = tokens[0];
    (void)fd;

	CmdMapIterator it = cmd.find(command);
	if (it != cmd.end()) {
		(this->*(it->second))(fd, tokens);
	} else {
		// handling unknown command
		std::cout << "Unknown command: " << command << std::endl;
	}
}

// Registration utils

void	Server::checkRegistration(int fd) {
	if (clients[fd]->isLogged()) {
		if (!clients[fd]->getNickname().empty() && !clients[fd]->getUsername().empty()) {
			clients[fd]->setRegistration();
			serverReply(fd, clients[fd]->getNickname(), RPL_WECLOME);
			serverReply(fd, clients[fd]->getNickname(), RPL_YOURHOST);
			serverReply(fd, clients[fd]->getNickname(), RPL_CREATED);
			serverReply(fd, clients[fd]->getNickname(), RPL_MYINFO);
		}
	}
}

// Find the parameters in command (after ':')
std::string	Server::getParam(const std::vector<std::string>& tokens) {
	std::vector<std::string>::const_iterator it = tokens.begin();
	for (; it != tokens.end(); ++it) {
		if (it[0][0] == ':')
			break;
	}
	std::string	param;
	for (; it != tokens.end(); ++it) {
		param.append(*it);
		if (it + 1 != tokens.end())
			param.append(" ");
	}
	size_t	cRet = param.find(':');
		if (cRet != std::string::npos)
			param.erase(cRet, 1);
	if (!param.empty())
		return param;
	return "";
}

bool	Server::verifyUsername(int fd, const std::string &arg) {
	if (arg.empty()) {
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	return 0;
}

bool	Server::verifyNickname(int fd, const std::string &arg) {
	// look for illegal characters
	if (arg[0] == ':' || arg[0] == '$')
		return (serverReply(fd, arg, ERR_ERRONEUSNICKNAME), 1);
	std::string	ill = " ,*?!@.";
	for (int i = 0; i < 8; ++i) {
		if (arg.find(ill[i]) != std::string::npos)
			return (serverReply(fd, arg, ERR_ERRONEUSNICKNAME), 1);
	}

	// check if username is already used
	std::map<int, Client *>::iterator it = clients.begin();
	for (; it != clients.end(); ++it) {
		if (it->second != clients[fd] && it->second->getNickname() == arg) {
			serverReply(fd, arg, ERR_NICKNAMEINUSE);
			return 1;
		}
	}
	return 0;
}

bool	Server::verifyPassword(int fd, const std::string &arg) {
	if (arg != password) {
		serverReply(fd, clients[fd]->getNickname(), ERR_PASSWDMISMATCH);
		return 1;
	}
	clients[fd]->setLog();
	return 0;
}

// Server reply

void	Server::serverReply(int fd, const std::string& token, serverRep id) {
	switch (id) {
		case CAPLS:
			serverSendReply(fd, "CAP_LS", token, "[...]");
			break;
		case RPL_WECLOME:
				serverSendReply(fd, "001", token, "Welcome to the IRC Network, " + token);
			break;
		case RPL_YOURHOST:
			serverSendReply(fd, "002", token, "Your host is " + serverName + ", running version " + serverVersion);
			break;
		case RPL_CREATED:
			serverSendReply(fd, "003", token, "This server was created " + static_cast<std::string>(ctime(&start)));
			break;
		case RPL_MYINFO:
			serverSendReply(fd, "004", token, "INFOS [...]");
			break;
		case ERR_NEEDMOREPARAMS:
			serverSendReply(fd, "461", token, "Not enough parameters");
			break;
		case ERR_PASSWDMISMATCH:
			serverSendReply(fd, "464", token, "Password incorrect");
			break;
		case ERR_NICKNAMEINUSE:
			serverSendReply(fd, "433", token, "Nickname is already in use");
			break;
		case ERR_ERRONEUSNICKNAME:
			serverSendReply(fd, "432", token, "Erroneous nickname");
			break;
		default:
			return;
	}
}

void    Server::serverSendReply(int fd, std::string id, const std::string& token, std::string reply) {
	std::stringstream fullReply;
	fullReply << ":" << serverName << " " << id << " " << token << " :" << reply << "\r\n";
	std::string replyStr = fullReply.str();
	send(fd, replyStr.c_str(), replyStr.length(), 0);
}
