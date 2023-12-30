#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>
#include <sstream>
#include <cstring>
#include <sys/socket.h>

#include "Server.hpp"

#define TOPICSET     0b000001 // if set topic is settable by channel operator only
#define INVITEONLY      0b000010 // if set clients can join only if invited
#define KEYSET            0b000100 // if set clients can join only with password
#define LIMITSET        0b001000 // if set no more clients than limit value can join

enum chanRep {
	RPL_TOPIC,
	RPL_NOTOPIC,
	RPL_NAMREPLY
};

class Channel {

private:
	Server *_server;
	std::string _name;
	std::string _topic;
	unsigned int _mode;
	std::string _password;
	std::string _visibility;
	std::set<int> _memberFds;
	std::set<int> _operatorFds;
	std::set<int> _invitedFds;
	int limitMembers;

public:

	Channel(const std::string &name, std::string &password);

	Channel(const std::string &name, Server *server);

	Channel(const std::string &name, std::string &password, Server *server);

	~Channel();


	const std::string &getName() const;

	const std::string &getTopic() const;

	unsigned int getMode() const;

	std::string getModeString() const;

	std::string getModeStringWithParameters() const;

	const std::set<int> &getMemberFds() const;

	const std::set<int> &getOperatorFds() const;

	int getLimitMembers() const;

	void setTopic(const std::string &topic);

	void setPassword(const std::string &password);

	void setLimitMembers(int limitMembers);

	bool setMode(unsigned int mode);

	bool unsetMode(unsigned int mode);

	bool isModeSet(unsigned int mode) const;


	void addMember(int clientFd);

	void removeMember(int clientFd);

	bool hasMember(int clientFd);

	bool addOperator(int clientFd);

	bool removeOperator(int clientFd);

	bool hasOperator(int clientFd);

	void addInvited(int clientFd);

	void removeInvited(int clientFd);

	bool hasInvited(int clientFd);

	void newMember(int fd);

	void chanReply(int fd, chanRep id);

	void chanSendReply(int fd, std::string id, const std::string &token, const std::string &reply);

	bool authMember(int clientFd, std::string &password);

};


#endif
