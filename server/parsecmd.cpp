#include "server.hpp"

std::vector<std::string> server::parseParams(const std::string &line, std::string &command, std::string *prefix)
{
    std::vector<std::string> params;
    size_t pos = 0;
    size_t len = line.size();

    command.clear();
    if (prefix)
        prefix->clear();
    if (pos < len && line[pos] == ':')
    {
        size_t sp = line.find(' ', pos);
        if (sp == std::string::npos)
            return params;
        if (prefix)
            *prefix = line.substr(pos + 1, sp - pos - 1);
        pos = sp + 1;
    }

    if (pos < len && line[pos] == ':')
    {
        size_t sp = line.find(' ', pos);
        if (sp == std::string::npos)
            return params;
        pos = sp + 1;
    }

    while (pos < len && line[pos] == ' ')
        pos++;

    size_t sp = line.find(' ', pos);
    if (sp == std::string::npos)
    {
        command = line.substr(pos);
        return params;
    }
    command = line.substr(pos, sp - pos);
    pos = sp + 1;

    while (pos < len)
    {
        while (pos < len && line[pos] == ' ')
            pos++;
        if (pos >= len)
            break;
        if (line[pos] == ':')
        {
            params.push_back(line.substr(pos + 1));
            break;
        }
        size_t next = line.find(' ', pos);
        if (next == std::string::npos)
        {
            params.push_back(line.substr(pos));
            break;
        }
        params.push_back(line.substr(pos, next - pos));
        pos = next + 1;
    }
    return params;
}

//---------------------- command dispatcher -----------------------//
void server::handleCommand(client *cl, const std::string &rawLine)
{
    std::string line = rawLine;

    while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
        line.erase(line.size() - 1);
    if (line.empty() || cl == NULL)
        return;

    std::string command;
    std::vector<std::string> params = parseParams(line, command);

    // for (size_t i = 0; i < command.size(); ++i)//we shuld remove this 
    //     command[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(command[i])));

    if (command.empty())
        return;
    else if (command == "PASS")
        pass(cl, params, this->password);
    else if (command == "NICK")
        nick(cl, params);
    else if (command == "USER")
        user(cl, params, line);
    else if (command == "JOIN")
        join(cl, params);
    else if (command == "MODE")
        mode(cl, params);
    else if (command == "PRIVMSG")
        privmsg(cl, params);
    else if (command == "KICK")
        kick(cl, params);
    else if (command == "INVITE")
        invite(cl, params);
    else if (command == "TOPIC")
        topic(cl, params);
    else
        sendReply(cl, "421", command, "Unknown command");
}