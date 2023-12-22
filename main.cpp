#include "Server.hpp"
#include "Channel.hpp"
bool running = true;

void signalHandler(int signum) {
	std::cout << "[SIG:" << signum << "] received.\n";
	running = false;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "ERROR! Usage: " << argv[0] << " <port> <_password>" << std::endl;
        return 1;
    }
    try {
        Server server(atoi(argv[1]), std::string(argv[2]));
		signal(SIGINT, signalHandler);
        while (running)
			server.run();
    } catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
