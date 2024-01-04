#include "Server.hpp"

// Parsing

bool Server::parsBuffer(int fd) {
	std::string bufferStr(this->_buffer);
	std::stringstream ss(bufferStr);
	std::string line;

//	 add recvBuffer at the beginning of ss if not empty
	try {
		Client &client = getClient(fd);
		if (!client.isRecvBufferEmpty()) {
			ss.str("");
			ss << client.getRecvBuffer() << bufferStr;
			client.resetRecvBuffer();
		}
		// check if buffer contains a full line
		size_t cRet = ss.str().find("\n");
		if (cRet == std::string::npos) {
			// if not, add buffer to recvBuffer and return
			client.appendRecvBuffer(ss.str());
			return 0;
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
//	 tokenize the buffer line by line
	while (std::getline(ss, line)) {
		size_t cRet = line.find('\r');
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
		if (!clients[fd]->isRegistered()) {
			if (registrationProcess(fd, tokens))
				return 1;
		} else
			processCmd(fd, tokens);
	}
	return 0;
}

bool Server::registrationProcess(int fd, std::vector<std::string> &tokens) {
	if (tokens.empty())
		return 0; // or 1?
	if (tokens.size() < 2) {
		serverSendError(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	std::string command = tokens[0];
	std::vector<std::string> params(tokens.begin() + 1, tokens.end());
	if (command == "CAP") {
		if (tokens[1] == "LS") {
			serverSendReply(fd, "", CAPLS, "");
		}
	} else if (handleCommand(fd, command, params)) {
		return 1;
	}
	return checkRegistration(fd);
}

bool Server::handleCommand(int fd, const std::string &command, const std::vector<std::string> &params) {
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
		std::string realname = getParam(params);
		if (verifyUsername(fd, realname))
			return 1;
		else
			clients[fd]->setUsername(realname);
	}
	return 0;
}

void Server::processCmd(int fd, std::vector<std::string> &tokens) {
	if (tokens.empty())
		return;

	std::string command = tokens[0];
	CmdIterator it = cmd.find(command);
	if (it != cmd.end()) {
		(this->*(it->second))(fd, tokens);
	} else {
		serverSendError(fd, command, ERR_UNKNOWNCOMMAND);
	}
}

// Registration utils

bool Server::checkRegistration(int fd) {
	// check if logging is complete
	if (clients[fd]->isLogged() && (!clients[fd]->getNickname().empty() && !clients[fd]->getUsername().empty())) {
		// check if user (nickname) is in database
		if (users.find(clients[fd]->getNickname()) != users.end()) {
			// check if user is already connected
			std::map<int, Client *>::iterator it = clients.begin();
			for (; it != clients.end(); ++it) {
				if (it->second != clients[fd] && it->second->getNickname() == clients[fd]->getNickname()) {
					serverSendError(fd, clients[fd]->getNickname(), ERR_NICKNAMEINUSE);
					return 1;
				}
			}
			// check if _password match previous login
			if (clients[fd]->getPassword() != users[clients[fd]->getNickname()]) {
				serverSendError(fd, clients[fd]->getNickname(), ERR_PASSWDMISMATCH);
				return 1;
			}
		} else {
			// if first time user connect: add to database
			users.insert(std::make_pair(clients[fd]->getNickname(), clients[fd]->getPassword()));
		}
		// resgitration complete, send welcome
		clients[fd]->setRegistration();
		serverSendReply(fd, "", RPL_WELCOME, "");
		serverSendReply(fd, "", RPL_YOURHOST, "");
		serverSendReply(fd, "", RPL_CREATED, "");
		serverSendReply(fd, "", RPL_MYINFO, "");
	}
	return 0;
}

// Find the parameters in command (after ':')
std::string Server::getParam(const std::vector<std::string> &tokens) {
	std::vector<std::string>::const_iterator it = tokens.begin();
	for (; it != tokens.end(); ++it) {
		if (it[0][0] == ':')
			break;
	}
	std::string param;
	for (; it != tokens.end(); ++it) {
		param.append(*it);
		if (it + 1 != tokens.end())
			param.append(" ");
	}
	size_t cRet = param.find(':');
	if (cRet != std::string::npos)
		param.erase(cRet, 1);
	if (!param.empty())
		return param;
	return "";
}

bool Server::verifyUsername(int fd, const std::string &arg) {
	if (arg.empty()) {
		serverSendError(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	return 0;
}

bool Server::verifyNickname(int fd, const std::string &arg) {
	// look for illegal characters
	if (arg[0] == ':' || arg[0] == '$')
		return (serverSendError(fd, arg, ERR_ERRONEUSNICKNAME), 1);
	std::string ill = " ,*?!@.";
	for (int i = 0; i < 8; ++i) {
		if (arg.find(ill[i]) != std::string::npos)
			return (serverSendError(fd, arg, ERR_ERRONEUSNICKNAME), 1);
	}
	return 0;
}

bool Server::verifyPassword(int fd, const std::string &arg) {
	if (arg.empty()) {
		serverSendError(fd, "", ERR_NEEDMOREPARAMS);
		return 1;
	}
	clients[fd]->setLog();
	return 0;
}

// Server reply

void Server::serverSendError(int fd, const std::string &token, serverRep id) {
	std::stringstream fullReply;
	fullReply << ":" << serverName << " " << paddDigits(id) << " " << getNick(fd);
	if (!token.empty()) {
		fullReply << " " << token;
	}
	fullReply << _serverMessages[id] << "\r\n";
	serverSendMessage(fd, fullReply.str());
}

void Server::serverSendReply(int fd, const std::string &token, serverRep id, const std::string &reply) {
	std::stringstream fullReply;
	if (id == CAPLS) { // todo: ??
		fullReply << ":" << serverName << " " << "CAP_LS" << " " << "[...]" << "\r\n";
		serverSendMessage(fd, fullReply.str());
		return;
	}
	fullReply << ":" << serverName << " " << paddDigits(id) << " " << getNick(fd);
	if (!token.empty()) {
		fullReply << " " << token;
	}
	if (_serverMessages.find(id) != _serverMessages.end()) {
		fullReply << _serverMessages[id];
	}
	if (!reply.empty()) {
		fullReply << " :" << reply;
	}
	fullReply << "\r\n";
	serverSendMessage(fd, fullReply.str());
}

void Server::serverSendNotification(int fd, const std::string &prefix, const std::string &command,
									const std::string &parameters) {
	std::stringstream fullNotification;
	fullNotification << ":" << prefix << " " << command << " " << parameters << "\r\n";
	serverSendMessage(fd, fullNotification.str());
}

void Server::serverSendNotification(const std::set<int> &fds, const std::string &prefix, const std::string &command,
									const std::string &parameters) {
	std::stringstream fullNotification;
	fullNotification << ":" << prefix << " " << command << " " << parameters << "\r\n";
	std::string notificationStr = fullNotification.str();

	for (std::set<int>::const_iterator it = fds.begin(); it != fds.end(); ++it) {
		serverSendMessage(*it, notificationStr);
	}
}

void Server::serverSendMessage(int fd, const std::string &message) {
	try {
		getClient(fd).pushSendQueue(message);
	} catch (std::exception &e) {
		std::cout << "[ERR] " << e.what() << std::endl;
	}
}

std::string Server::paddDigits(int i) {
    std::ostringstream  stream;
    stream << std::setw(3) << std::setfill('0') << i;
    return stream.str();
}

