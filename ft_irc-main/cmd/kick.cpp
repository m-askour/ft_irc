#include "cmd.hpp"
#include "./server/server.hpp"

// KICK <#channel> <nickname> [:reason]
void server::kick(client *cl, const std::vector<std::string> &args)
{
    if (args.size() < 2 || args[0].empty() || args[1].empty())
    {
        sendReply(cl, "461", "KICK", "Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    std::string targetNick = args[1];
    std::string reason = (args.size() >= 3 && !args[2].empty()) ? args[2] : cl->get_nickname();

    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        sendReply(cl, "403", channelName, "No such channel");
        return;
    }
    Channel *channel = it->second;

    if (channel->users.find(cl) == channel->users.end())
    {
        sendReply(cl, "442", channelName, "You're not on that channel");
        return;
    }
    if (channel->operators.find(cl) == channel->operators.end())
    {
        sendReply(cl, "482", channelName, "You're not channel operator");
        return;
    }

    client *target = NULL;
    for (std::set<client*>::iterator memberIt = channel->users.begin(); memberIt != channel->users.end(); ++memberIt)
    {
        if ((*memberIt)->get_nickname() == targetNick)
        {
            target = *memberIt;
            break;
        }
    }
    if (!target)
    {
        sendReply(cl, "441", targetNick, "They aren't on that channel");
        return;
    }

    std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
    std::string kickMsg = ":" + prefix + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";

    broadcastToChannel(channel, kickMsg, NULL);

    channel->users.erase(target);
    channel->operators.erase(target);
    channel->invitedUsers.erase(target);
}
