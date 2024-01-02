#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <set>
#include <queue>

// TODO : Rework to make a binary field instead of a set
enum Mode {
    AWAY, // a: user is flagged as away
    INVISIBLE, // i: marks a users as invisible
    WALLOPS, // w: user receives wallops
    RESTRICTED, // r: restricted user connection
    OPERATOR, // o: operator flag
    LOCAL_OPERATOR, // O: local operator flag
    SERVER_NOTICES, // s: marks a user for receipt of server notices
    UNKNOWN
};

class Client {
    private:
        int         socketFd;
        bool        logged;
        bool        registered;
        bool        isOpe;
        std::string nickname;
        std::string username;
        std::string realName;
        std::string password;
        std::set<Mode> modes;
		std::string		recvBuffer;
		std::queue<std::string> _sendQueue;
		std::string _awayMessage;

    public:
        Client(int socket);
        ~Client();
        const std::string &getNickname() const;
        const std::string &getUsername() const;
        const std::string &getPassword() const;
        int getSocket();
		const std::string &getAwayMessage() const;
		bool isRegistered();
        bool isOperator();
        bool isLogged();
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setPassword(const std::string &password);
        void setRegistration();
        void setLog();
        void addMode(Mode mode);
        void removeMode(Mode mode);
        bool activeMode(Mode mode) const;
        Mode getMode(const std::string &mode);
		std::string getRecvBuffer();
		void appendRecvBuffer(std::string recv);
		void resetRecvBuffer();
		bool isRecvBufferEmpty();
		void pushSendQueue(std::string send);
		std::string popSendQueue();
		bool sendQueueEmpty();
};

#endif
