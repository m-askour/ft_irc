#include "channel.hpp"

void server::kick(client *cl, const std::vector<std::string> &args)
{
	if (!cl->is_authenticated_client() || !cl->is_registered_client() || !cl->welcom_to_server())
		return sendReply(cl, "451", "", "You have not registered");
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return sendReply(cl, "461", "KICK", "Not enough parameters");

	std::vector<std::string> channelNames = split(args[0], ',');
	std::vector<std::string> targetNicks = split(args[1], ',');
	if (channelNames.empty() || targetNicks.empty())
		return sendReply(cl, "461", "KICK", "Not enough parameters");
	if (channelNames.size() != 1 && channelNames.size() != targetNicks.size())
		return sendReply(cl, "461", "KICK", "Not enough parameters");

	std::string reason;
	if (args.size() >= 3 && !args[2].empty())
	{
		if (args[2][0] == ':')
			reason = args[2].substr(1);
		else
			reason = args[2];
	}
	else
		reason = cl->get_nickname();

	for (size_t i = 0; i < targetNicks.size(); ++i)
	{
		std::string &channelName = channelNames.size() == 1 ? channelNames[0] : channelNames[i];
		std::string &targetNick = targetNicks[i];
		std::map<std::string, Channel *>::iterator it = channels.find(channelName);
		if (it == channels.end())
		{
			sendReply(cl, "403", channelName, "No such channel");
			continue;
		}
		Channel *channel = it->second;
		if (channel->users.find(cl) == channel->users.end())
		{
			sendReply(cl, "442", channelName, "You're not on that channel");
			continue;
		}
		if (channel->operators.find(cl) == channel->operators.end())
		{
			sendReply(cl, "482", channelName, "You're not channel operator");
			continue;
		}

		client *target = NULL;
		for (std::set<client *>::iterator memberIt = channel->users.begin(); memberIt != channel->users.end(); ++memberIt)
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
			continue;
		}
		std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@" + cl->get_client_ip();
		std::string kickMsg = ":" + prefix + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
		broadcastToChannel(channel, kickMsg, NULL);
		channel->users.erase(target);
		channel->operators.erase(target);
		channel->invitedUsers.erase(target);
		if (channel->users.empty())
		{
			delete channel;
			channels.erase(channelName);
		}
		else if (channel->operators.empty())
		{
			client *newOperator = *(channel->users.begin());
			channel->operators.insert(newOperator);
			sendReply(newOperator, "MODE", channelName + " +o " + newOperator->get_nickname(), "You are now a channel operator");
		}
	}
}
