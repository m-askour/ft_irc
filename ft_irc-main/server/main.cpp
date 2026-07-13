#include "server.hpp"
#include <csignal>
#include <cstdlib>

static bool parsePort(const std::string &arg, int &port)
{
    if (arg.empty() || arg.find_first_not_of("0123456789") != std::string::npos)
        return false;

    long value = std::strtol(arg.c_str(), NULL, 10);
    if (value <= 0 || value > 65535)
        return false;

    port = static_cast<int>(value);
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port;
    if (!parsePort(argv[1], port))
    {
        std::cerr << "Error: port must be a number between 1 and 65535" << std::endl;
        return 1;
    }

    std::string password(argv[2]);
    if (password.empty())
    {
        std::cerr << "Error: password cannot be empty" << std::endl;
        return 1;
    }

    // Ignore SIGPIPE so a client disconnecting mid-send() doesn't kill the server.
    signal(SIGPIPE, SIG_IGN);

    server irc(AF_INET, SOCK_STREAM, 0);
    irc.start_server(port, password);

    return 0;
}