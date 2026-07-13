#include "cmd.hpp"
#include "./server/server.hpp"

// INVITE <nickname> <#channel>
void server::invite(client *cl, const std::vector<std::string> &args)
{
    if (args.size() < 2 || args[0].empty() || args[1].empty())
    {
        sendReply(cl, "461", "INVITE", "Not enough parameters");
        return;
    }

    std::string targetNick = args[0];
    std::string channelName = args[1];

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

    // Only channel operators may invite while the channel is invite-only (+i).
    if (channel->inviteOnly && channel->operators.find(cl) == channel->operators.end())
    {
        sendReply(cl, "482", channelName, "You're not channel operator");
        return;
    }

    client *target = NULL;
    for (std::map<int, client*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2)
    {
        if (it2->second->get_nickname() == targetNick)
        {
            target = it2->second;
            break;
        }
    }
    if (!target)
    {
        sendReply(cl, "401", targetNick, "No such nick/channel");
        return;
    }

    if (channel->users.find(target) != channel->users.end())
    {
        sendReply(cl, "443", targetNick + " " + channelName, "is already on channel");
        return;
    }

    channel->invitedUsers.insert(target);

    // Confirm to the inviter.
    sendReply(cl, "341", targetNick, channelName);

    // Notify the invited user.
    std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
    std::string inviteMsg = ":" + prefix + " INVITE " + targetNick + " :" + channelName + "\r\n";
    target->send_data(inviteMsg);
}
