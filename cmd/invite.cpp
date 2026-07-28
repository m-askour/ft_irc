#include "channel.hpp"

void server::invite(client *cl, const std::vector<std::string> &args)
{
	if (!cl->is_authenticated_client() || !cl->is_registered_client() || !cl->welcom_to_server())
		return sendReply(cl, "451", "", "You have not registered");
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return sendReply(cl, "461", "INVITE", "Not enough parameters");

	std::string targetNick = args[0];
	std::string channelName = args[1];
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);
	Channel *channel = (it != channels.end()) ? it->second : NULL;
	if (channel)
	{
		if (channel->users.find(cl) == channel->users.end())
			return sendReply(cl, "442", channelName, "You're not on that channel");
		if (channel->inviteOnly && channel->operators.find(cl) == channel->operators.end())
			return sendReply(cl, "482", channelName, "You're not channel operator");
	}

	client *target = NULL;
	for (std::map<int, client *>::iterator it2 = clients.begin(); it2 != clients.end(); it2++)
	{
		if (it2->second->get_nickname() == targetNick)
		{
			target = it2->second;
			break;
		}
	}
	if (!target)
		return sendReply(cl, "401", targetNick, "No such nick/channel");
	if (channel && channel->users.find(target) != channel->users.end())
		return sendReply(cl, "443", targetNick + " " + channelName, "is already on channel");
	if (channel)
		channel->invitedUsers.insert(target);

	sendReply(cl, "341", targetNick, channelName);

	std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
	std::string inviteMsg = ":" + prefix + " INVITE " + targetNick + " :" + channelName + "\r\n";
	target->send_data(inviteMsg);
}
