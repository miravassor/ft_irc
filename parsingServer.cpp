#include "Server.hpp"

// Parsing

bool	Server::parsBuffer(int fd) {
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
			std::cout << token << " "; // debug
			tokens.push_back(token);
		}
		std::cout << "[TOKEN END]" << std::endl; // debug
		if (clients[fd]->isRegistered() == false) {
			if (registrationProcess(fd, tokens))
				return 1;
		}
		else
			processCmd(fd, tokens);
	}
	return 0;
}

bool	Server::registrationProcess(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
			return 0; // or 1?
	if (tokens.size() < 2) {
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	std::string command = tokens[0];
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());
	if (command == "CAP") {
		if (tokens[1] == "LS") {
			serverReply(fd, "", CAPLS);
		}
	}
	else if (handleCommand(fd, command, params)) {
		return 1;
	}
	return checkRegistration(fd);
}

bool	Server::handleCommand(int fd, const std::string& command, const std::vector<std::string>& params) {
	if (command == "PASS") {
		if (verifyPassword(fd, params[0]))
			return 1;
		else
			clients[fd]->setPassword(params[0]);
	} else if (command == "NICK") {
		if (verifyNickname(fd, params[0]))
			return 1;
		else
			clients[fd]->setNickname(params[0]);
	} else if (command == "USER") {
		std::string	realname = getParam(params);
		if (verifyUsername(fd, realname))
			return 1;
		else
			clients[fd]->setUsername(realname);
	}
	return 0;
}

void Server::processCmd(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
		return;
	if (tokens.size() < 2) {
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
		return;
	}

	std::string command = tokens[0];
	CmdMapIterator it = cmd.find(command);
	if (it != cmd.end()) {
		(this->*(it->second))(fd, tokens);
	} else {
		// handling unknown command
		std::cout << "Unknown command: " << command << std::endl;
	}
}

// Registration utils

bool	Server::checkRegistration(int fd) {
	// check if logging is complete
	if (clients[fd]->isLogged() && (!clients[fd]->getNickname().empty() && !clients[fd]->getUsername().empty()) ) {
		// check if user (nickname) is in database
		if (users.find(clients[fd]->getNickname()) != users.end()) {
			// check if user is already connected
			std::map<int, Client *>::iterator it = clients.begin();
			for (; it != clients.end(); ++it) {
				if (it->second != clients[fd] && it->second->getNickname() == clients[fd]->getNickname()) {
					serverReply(fd, clients[fd]->getNickname(), ERR_NICKNAMEINUSE);
					return 1;
				}
			}
			// check if _password match previous login
			if (clients[fd]->getPassword() != users[clients[fd]->getNickname()]) {
				serverReply(fd, clients[fd]->getNickname(), ERR_PASSWDMISMATCH);
				return 1;
			}
		}
		else {
			// if first time user connect: add to database
			users.insert(std::make_pair(clients[fd]->getNickname(), clients[fd]->getPassword()));
		}
		// resgitration complete, send welcome
		clients[fd]->setRegistration();
		serverReply(fd, clients[fd]->getNickname(), RPL_WECLOME);
		serverReply(fd, clients[fd]->getNickname(), RPL_YOURHOST);
		serverReply(fd, clients[fd]->getNickname(), RPL_CREATED);
		serverReply(fd, clients[fd]->getNickname(), RPL_MYINFO);
	}
	return 0;
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
	return 0;
}

bool	Server::verifyPassword(int fd, const std::string &arg) {
	if (arg.empty()) {
		serverReply(fd, "", ERR_NEEDMOREPARAMS);
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
		case ERR_ALREADYREGISTRED:
			serverSendReply(fd, "462", token, "Unauthorized command (already registered)");
			break;
		case ERR_USERSDONTMATCH:
			serverSendReply(fd, "502", token, "Cannot change _mode for other users");
			break;
		case RPL_UMODEIS:
			serverSendReply(fd, "221", token, token);
			break;
		case ERR_UMODEUNKNOWNFLAG:
			serverSendReply(fd, "501", token, "Unknown MODE flag");
			break;
		case PONG:
			send(fd, token.c_str(), token.size(), 0);
			break;
		case ERR_NOSUCHSERVER:
			serverSendReply(fd, "402", token, "No such server");
			break;
		case ERR_NOORIGIN:
			serverSendReply(fd, "409", "", "No origin specified");
			break;
		case ERR_BADCHANNELKEY:
			serverSendReply(fd, "475", token, "Cannot join channel");
			break;
		case ERR_NOSUCHCHANNEL:
			serverSendReply(fd, "403", token, "No such channel");
			break;
		case ERR_NOTONCHANNEL:
			serverSendReply(fd, "442", token, "You're not on that channel");
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
