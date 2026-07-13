#include "cmd.hpp"
#include "./server/server.hpp"


bool isValidChannelName(const std::string &name)
{
    if (name.empty() || (name[0] != '#' && name[0] != '&'))
        return false;
    if (name.size() > 200)
        return false;
    for (size_t i = 0; i < name.size(); ++i)
    {
        char c = name[i];
        if (c == ' ' || c == ',' || c == '\a' || c == '\0')
            return false;
    }
    return true;
}

std::vector<std::string> server::split(const std::string &s, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(s);

    while (std::getline(stream, token, delim))
        tokens.push_back(token);

    return tokens;
}

void server::joinOneChannel(client *client, const std::string &channelName, const std::string &key)
{
    //chceck this wher u can put it 
    if (!isValidChannelName(channelName))
    {
        sendReply(client, "403", channelName, "No such channel");
    }
    Channel *channel;
    bool isNew = (channels.find(channelName) == channels.end());

    if (isNew)
    {
        channel = new Channel();
        channel->name = channelName;
        channel->keyEnabled = false;
        channel->inviteOnly = false;
        channel->limitEnabled = false;
        channel->topicProtected = false;
        channel->userLimit = 0;
        channels[channelName] = channel;
        channel->operators.insert(client);
    }
    else
    {
        channel = channels[channelName];

        if (channel->inviteOnly && channel->invitedUsers.find(client) == channel->invitedUsers.end())
        {
            sendReply(client, "473", channelName, "Cannot join channel (+i)");
            return;
        }
        if (channel->keyEnabled && key != channel->password)
        {
            sendReply(client, "475", channelName, "Cannot join channel (+k)");
            return;
        }
        if (channel->limitEnabled && channel->users.size() >= channel->userLimit)
        {
            sendReply(client, "471", channelName, "Cannot join channel (+l)");
            return;
        }
        if (channel->users.find(client) != channel->users.end())
            return;
    }
    channel->users.insert(client);
    std::cout << "channel name :" << channel->name << std::endl;
    notifyJoin(client, channel);
}

void server::join(client *client, std::vector<std::string> args)
{
    std::vector<std::string> channelNames;
    std::vector<std::string> keys;

    if (args.empty())
    {
        sendReply(client, "461", "JOIN", "Not enough parameters");
        return;
    }
    channelNames = split(args[0], ',');
    if (args.size() > 1)
        keys = split(args[1], ',');
    for (size_t i = 0; i < channelNames.size(); ++i)
    {
        std::string key = (i < keys.size()) ? keys[i] : "";
        joinOneChannel(client, channelNames[i], key);
    }
}