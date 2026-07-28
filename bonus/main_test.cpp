#include "bot.hpp"
#include <cstdlib>

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cout << "Usage: ./ircbot <server_ip> <server_port> <server_password> [channel]" << std::endl;
        return 1;
    }

    std::string server_ip = argv[1];
    int server_port = std::atoi(argv[2]);
    std::string server_password = argv[3];
    std::string channel = "#" + std::string(argv[4]);
    bot ircBot("Ircbot", channel);

    if (!ircBot.connectToServer(server_ip, server_port, server_password))
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    ircBot.communicateWithServer();
    return 0;
}