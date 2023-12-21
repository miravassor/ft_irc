#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <map>
#include <vector>
#include <poll.h>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <queue>

#include "Client.hpp"

class Channel;

enum serverRep {
	CAPLS,
	RPL_WECLOME,
	RPL_YOURHOST,
	RPL_CREATED,
	RPL_MYINFO,
	ERR_NEEDMOREPARAMS,
	ERR_PASSWDMISMATCH,
	ERR_NICKNAMEINUSE,
	ERR_ERRONEUSNICKNAME,
    ERR_ALREADYREGISTRED,
    ERR_USERSDONTMATCH,
    RPL_UMODEIS,
    ERR_UMODEUNKNOWNFLAG,
    PONG,
    ERR_NOSUCHSERVER,
    ERR_NOORIGIN,
  	ERR_NOSUCHCHANNEL,
  	ERR_BADCHANNELKEY
};

class Server {
 	public:
  		typedef std::map<std::string, void (Server::*)(int, const std::vector<std::string>&)> CmdMap;
  		typedef std::map<std::string, void (Server::*)(int, const std::vector<std::string>&)>::iterator CmdMapIterator;

        Server() {};
  		Server(int port, const std::string &password);
  		~Server();
  		void run();

        // Channel getters
        std::string getServerName();
        std::string getNick(int fd);
        std::string simpleSend(std::string send);

    private:
        int socketFd;
        time_t start;
        sockaddr_in address;
        std::string _password;
        std::string serverName;
        std::string serverVersion;
        std::vector<pollfd> pollFds;
        std::map<int, Client *> clients;
        std::vector<Channel *> _channels;
  		CmdMap cmd;
        std::map<std::string, std::string> users;

		void  initCmd();

        void addClient(int clientSocket);
        void removeClient(int clientSocket);
        void listenPort() const;
        int acceptConnection();

        // Parsing
        char _buffer[1024];
        bool    parsBuffer(int fd);
        bool    registrationProcess(int fd, std::vector<std::string>& tokens);
        bool    checkRegistration(int fd);
        bool    handleCommand(int fd, const std::string& command, const std::vector<std::string>& params );
        bool    verifyNickname(int fd, const std::string &arg);
        bool    verifyPassword(int fd, const std::string &arg);
        bool    verifyUsername(int fd, const std::string &arg);
        void    processCmd(int fd, std::vector<std::string>& tokens);
        void    serverReply(int fd, const std::string& token, serverRep id);
        void    serverSendReply(int fd, std::string id, const std::string& token, std::string reply);
        std::string getParam(const std::vector<std::string>& tokens);

        // Commands
		void processPrivmsg(int fd, const std::vector<std::string> &tokens);
  		void processJoin(int fd, const std::vector<std::string> &tokens);
  		void processInvite(int fd, const std::vector<std::string> &tokens);
  		void processKick(int fd, const std::vector<std::string> &tokens);
  		void processPart(int fd, const std::vector<std::string> &tokens);
        void processMode(int fd, const std::vector<std::string> &tokens);
        void processPing(int fd, const std::vector<std::string> &tokens);

		void addChannel(Channel *channel);
		Channel *findChannel(const std::string &name);

  		bool isValidChannelName(const std::string &name);
  		std::queue<std::string> split(const std::string &src, char delimiter) const;
};

#endif
