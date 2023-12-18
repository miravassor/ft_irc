#include "Server.hpp"

bool	Server::registrationProcess(int fd, std::vector<std::string>& tokens) {
	if (tokens.empty())
			return 0; // or 1?

	std::string command = tokens[0];
	if (command == "CAP") {
		if (tokens[1] == "LS") {
			std::string	capls = "Entering registration process\r\n";
			send(fd, capls.c_str(), capls.length(), 0);
		}
	}
	else if (command == "PASS") {
		if (tokens[1] == password)
			clients[fd]->setLog();
		else {
			std::string	wrongpass = "Wrong password, connection failed\r\n";
			send(fd, wrongpass.c_str(), wrongpass.length(), 0);
			removeClient(clients[fd]->getSocket());
			return 1;
		}
	}
	else if (command == "NICK") {
		if (!verifyNickname(fd, tokens[1]))
			clients[fd]->setNickname(tokens[1]);
		else {
			std::string	inuse = "Nickname already in use, connection failed\r\n";
			send(fd, inuse.c_str(), inuse.length(), 0);
			removeClient(clients[fd]->getSocket());
			return 1;
		}
	}
	else if (command == "USER") {
		clients[fd]->setUsername(tokens[1]);
	}
	checkRegistration(fd);
	return 0;
}

bool	Server::verifyNickname(int fd, const std::string &token) {
	// look for illegal characters
	if (token.empty())
		return 1;
	if (token.find(" ") != std::string::npos)
		return 1;
	else if (token.find(",") != std::string::npos)
		return 1;
	else if (token.find("*") != std::string::npos)
		return 1;
	else if	(token.find("?") != std::string::npos)
		return 1;
	else if	(token.find("!") != std::string::npos)
		return 1;
	else if	(token.find("@") != std::string::npos)
		return 1;
	else if	(token.find(".") != std::string::npos)
		return 1;
	else if (token[0] == '$' || token [0] == ':')
		return 1;

	// check if username is already used
	std::map<int, Client *>::iterator it = clients.begin();
	for (; it != clients.end(); ++it) {
		if (it->second != clients[fd] && it->second->getNickname() == token)
			return (1);
	}
	return (0);
}

void	Server::checkRegistration(int fd) {
	Client* client = clients[fd];

	if (client->isLogged()) {
		if (!client->getNickname().empty() && !client->getUsername().empty()) {
			client->setRegistration();
			serverReply(fd, client, RPL_WECLOME);
			serverReply(fd, client, RPL_YOURHOST);
			serverReply(fd, client, RPL_CREATED);
			serverReply(fd, client, RPL_MYINFO);
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

		if (clients[fd]->isRegistered() == false) {
			if (registrationProcess(fd, tokens))
				return;
		}
		else
			processCmd(fd, tokens);
	}
}

void Server::processCmd(int fd, std::vector<std::string>& tokens) {
    if (tokens.empty())
        return;

    std::string command = tokens[0];
    (void)fd;

	// draft template sending messages to multiple targets (channels, users, both)
    if (command == "PRIVMSG") {
        std::string message = tokens.back();
        for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; ++it) {
            std::string target = *it;
            if (target.at(0) == '#' || target.at(0) == '&') {
                // broadcastMessage(fd, target, message);
            } else {
                // privateMessage(fd, target, message);
            }
        }
    }
}

void    Server::serverReply(int fd, Client *client, serverRep id) {
    switch (id) {
        case RPL_WECLOME:
            serverSendReply(fd, "001", client->getNickname(), "Welcome to the IRC Network, " + client->getNickname());
            break;
        case RPL_YOURHOST:
            serverSendReply(fd, "002", client->getNickname(), "Your host is " + serverName + ", running version " + serverVersion);
            break;
        case RPL_CREATED:
            serverSendReply(fd, "003", client->getNickname(), "This server was created " + static_cast<std::string>(ctime(&start)));
            break;
        case RPL_MYINFO:
            serverSendReply(fd, "004", client->getNickname(), "INFOS [...]");
            break;
        default:
            return;
    }
}

void    Server::serverSendReply(int fd, std::string id, std::string nickname, std::string reply) {
    std::stringstream fullReply;
    fullReply << ":" << serverName << " " << id << " " << nickname << " :" << reply << "\r\n";
    std::string replyStr = fullReply.str();
    send(fd, replyStr.c_str(), replyStr.length(), 0);
}
