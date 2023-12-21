#include "Server.hpp"
#include "Channel.hpp"

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "ERROR! Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    try {
        Server server(atoi(argv[1]), std::string(argv[2]));
        server.run();
    } catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
