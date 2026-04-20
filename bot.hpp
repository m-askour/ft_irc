#ifndef BOT_HPP
#define BOT_HPP
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <vector>
class bot
{
private:

    std::string input;
    std::vector <std::string> history; 
    std::string response;

public:
    bot(/* args */);
    bot(const bot &other);
    bot &operator=(const bot &other);
    ~bot();

    void GetUserInput();
    std::string processinput(std::string &input);
    std::string getClientName(int socket_fd);
    bool keywordMatching(std::string &input, std::string &response);
    std::string failbackResponse();
};


#endif