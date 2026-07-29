#include "channel.hpp"

void server::topic(client *cl, const std::vector<std::string> &args)
{
	if (!cl->is_authenticated_client() || !cl->is_registered_client() || !cl->welcom_to_server())
		return sendReply(cl, "451", "", "You have not registered");
	if (args.empty() || args[0].empty())
		return sendReply(cl, "461", "TOPIC", "Not enough parameters");

	std::string channelName = args[0];
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	if (it == channels.end())
		return sendReply(cl, "403", channelName, "No such channel");

	Channel *channel = it->second;
	if (channel->users.find(cl) == channel->users.end())
		return sendReply(cl, "442", channelName, "You're not on that channel");
	if (args.size() < 2)
	{
		if (channel->topic.empty())
			return sendReply(cl, "331", channelName, "No topic is set");
		else
			return sendReply(cl, "332", channelName, channel->topic);
	}
	if (!channel->topicProtected && channel->operators.find(cl) == channel->operators.end())
		return sendReply(cl, "482", channelName, "You're not channel operator");

	if (args[1][0] == ':')
		channel->topic = args[1].substr(1);
	else
		channel->topic = args[1];
	for (size_t i = 2; i < args.size(); ++i)
		channel->topic += " " + args[i];
	std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@" + cl->get_client_ip();
	std::string topicMsg = ":" + prefix + " TOPIC " + channelName + " :" + channel->topic + "\r\n";
	broadcastToChannel(channel, topicMsg, NULL);
}
