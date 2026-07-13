#include "cmd.hpp"
#include "./server/server.hpp"

// TOPIC <#channel>            -> view current topic
// TOPIC <#channel> :<topic>   -> set a new topic (respects +t)
void server::topic(client *cl, const std::vector<std::string> &args)
{
    if (args.empty() || args[0].empty())
    {
        sendReply(cl, "461", "TOPIC", "Not enough parameters");
        return;
    }

    std::string channelName = args[0];
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

    // No second parameter: return the current topic.
    if (args.size() < 2)
    {
        if (channel->topic.empty())
            sendReply(cl, "331", channelName, "No topic is set");
        else
            sendReply(cl, "332", channelName, channel->topic);
        return;
    }

    // +t restricts topic changes to channel operators.
    if (channel->topicProtected && channel->operators.find(cl) == channel->operators.end())
    {
        sendReply(cl, "482", channelName, "You're not channel operator");
        return;
    }

    channel->topic = args[1];

    std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
    std::string topicMsg = ":" + prefix + " TOPIC " + channelName + " :" + channel->topic + "\r\n";
    broadcastToChannel(channel, topicMsg, NULL);
}
