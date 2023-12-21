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
		std::queue<std::string> _sendQueue;

    public:
        Client();
        ~Client();
        const std::string &getNickname() const;
        const std::string &getUsername() const;
        const std::string &getPassword() const;
        int getSocket();
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
		void pushSendQueue(std::string send);
		std::string popSendQueue();
		bool sendQueueEmpty();
};

#endif
