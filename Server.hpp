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
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <csignal>

#include "Client.hpp"
#include "Channel.hpp"
#include "rpl.hpp"
#include "err.hpp"

class Server {
public:
	typedef std::map<std::string, void (Server::*)(int, const std::vector<std::string> &)> Cmd;
	typedef std::map<std::string, void (Server::*)(int, const std::vector<std::string> &)>::iterator CmdIterator;
	typedef std::map<char, bool (Server::*)(char, const std::string &, Channel *, int)> ModeHandler;
	typedef std::map<char, bool (Server::*)(char, const std::string &, Channel *, int)>::iterator ModeHandlerIterator;

	Server() {};

	Server(int port, const std::string &password);

	~Server();

	void run();

	// Channel getters
	std::string getServerName();

	std::string getNick(int fd);

	std::vector<std::string> getNicknames(std::set<int> fds);

	std::string simpleSend(std::string send);

	Client &getClient(int fd);

	const std::map<int, Client *> &getClients() const;

	std::set<int> getClientsFds() const;

private:
	static std::map<int, std::string> rpl;
	static std::map<int, std::string> err;

	int socketFd;
	time_t start;
	sockaddr_in address;
	std::string _password;
	std::string serverName;
	std::string serverVersion;
	std::vector<pollfd> pollFds;
	std::map<int, Client *> clients;
	std::vector<Channel *> _channels;
	Cmd cmd;
	ModeHandler channelMode;
	std::map<std::string, std::string> users;

	void initCmd();

	void initChannelMode();


	Client *findClient(const std::string &nickname);

	void addClient(int clientSocket);

	void removeClient(int clientSocket);

	void listenPort() const;

	int acceptConnection();

	// Parsing
	char _buffer[1024];

	bool parsBuffer(int fd);

	bool registrationProcess(int fd, std::vector<std::string> &tokens);

	bool checkRegistration(int fd);

	bool handleCommand(int fd, const std::string &command, const std::vector<std::string> &params);

	bool verifyNickname(int fd, const std::string &arg);

	bool verifyPassword(int fd, const std::string &arg);

	bool verifyUsername(int fd, const std::string &arg);

	void processCmd(int fd, std::vector<std::string> &tokens);

	void serverReply(int fd, const std::string &token, int id);

	void serverSendReply(int fd, std::string id, const std::string &token, std::string reply);

	void serverSendError(int fd, std::string id, const std::string &token, std::string reply);

	void serverSendNotification(int fd, const std::string &prefix, const std::string &command,
	                            const std::string &parameters);

	void serverSendNotification(const std::set<int> &fds, const std::string &prefix, const std::string &command,
	                            const std::string &parameters);

	void serverSendMessage(int fd, const std::string &message);

	std::string getParam(const std::vector<std::string> &tokens);

	// Commands
	void processPrivmsg(int fd, const std::vector<std::string> &tokens);

	void processJoin(int fd, const std::vector<std::string> &tokens);

	void processInvite(int fd, const std::vector<std::string> &tokens);

	void processKick(int fd, const std::vector<std::string> &tokens);

	void processTopic(int fd, const std::vector<std::string> &tokens);

	void processPart(int fd, const std::vector<std::string> &tokens);

	void processMode(int fd, const std::vector<std::string> &tokens);

	void processChannelMode(int fd, const std::vector<std::string> &tokens);

	void processUserMode(int fd, const std::vector<std::string> &tokens);

	void processNames(int fd, const std::vector<std::string> &tokens);

	void processList(int fd, const std::vector<std::string> &tokens);

	void processPing(int fd, const std::vector<std::string> &tokens);

	bool handleModeT(char set, const std::string &parameter, Channel *channel, int fd);

	bool handleModeI(char set, const std::string &parameter, Channel *channel, int fd);

	bool handleModeK(char set, const std::string &parameter, Channel *channel, int fd);

	bool handleModeL(char set, const std::string &parameter, Channel *channel, int fd);

	bool handleModeO(char set, const std::string &parameter, Channel *channel, int fd);

	void addChannel(Channel *channel);

	Channel *findChannel(const std::string &name);

	std::vector<Channel *> findChannels(std::queue<std::string> names);

	std::vector<Channel *>::iterator findChannelIterator(const std::string &name);

	bool isValidChannelName(const std::string &name);

	std::queue<std::string> split(const std::string &src, char delimiter, bool unique) const;

	void sendData(size_t index);

	size_t receiveData(size_t index);

	void resetEvents(size_t index);

	void fillChannelsFds(std::map<std::string, std::set<int> > &channelsFds,
	                     std::pair<std::string, std::set<int> > *fdsWithoutChannels,
	                     std::vector<Channel *> &channels) const;

	std::string mergeTokensToString(const std::vector<std::string> &tokens);

	void sendJoinNotificationsAndReplies(int fd, const Channel *channel);

	bool checkPmTokens(int fd, const std::vector<std::string> &tokens);

	void sendPmToChan(int fd, const std::string &message, const std::string &prefix, const std::string &targetName,
	                  const std::string &command);

	void sendPmToUser(int fd, const std::string &message, const std::string &prefix, const std::string &targetName,
	                  const std::string &command);

	std::string capitalizeString(const std::string &input);

	void listChannels(int fd, std::vector<Channel *> &channels);

	bool modeParameterNeeded(char set, char mode);

	bool isValidName(const std::string &name);
};

#endif
