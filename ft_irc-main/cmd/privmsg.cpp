#include "cmd.hpp"
#include "./server/server.hpp"

// PRIVMSG <target> :<message>
// target can be a nickname (private message) or a channel name (broadcast).
void server::privmsg(client *cl, const std::vector<std::string> &args)
{
    if (args.empty() || args[0].empty())
    {
        sendReply(cl, "411", "", "No recipient given (PRIVMSG)");
        return;
    }
    if (args.size() < 2 || args[1].empty())
    {
        sendReply(cl, "412", "", "No text to send");
        return;
    }

    std::string target = args[0];
    std::string message = args[1];
    std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
    std::string fullMsg = ":" + prefix + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#' || target[0] == '&')
    {
        std::map<std::string, Channel*>::iterator it = channels.find(target);
        if (it == channels.end())
        {
            sendReply(cl, "403", target, "No such channel");
            return;
        }
        Channel *channel = it->second;

        if (channel->users.find(cl) == channel->users.end())
        {
            sendReply(cl, "404", target, "Cannot send to channel");
            return;
        }
        broadcastToChannel(channel, fullMsg, cl);
    }
    else
    {
        client *dest = NULL;
        for (std::map<int, client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second->get_nickname() == target)
            {
                dest = it->second;
                break;
            }
        }
        if (!dest)
        {
            sendReply(cl, "401", target, "No such nick/channel");
            return;
        }
        dest->send_data(fullMsg);
    }
}
