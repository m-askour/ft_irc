#ifndef BOT_HPP
#define BOT_HPP
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <algorithm>
#include <vector>

///  dont forget to inherit from the server to takck the name of server and the name of the client to know the name of each client 

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
    bool keywordMatching(std::string &input, std::string &response);
    std::string failbackResponse();
};


#endif