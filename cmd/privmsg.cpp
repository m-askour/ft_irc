#include "channel.hpp"

void server::privmsg(client *cl, const std::vector<std::string> &args)
{
	if (!cl->is_authenticated_client() || !cl->is_registered_client() || !cl->welcom_to_server())
		return sendReply(cl, "451", "", "You have not registered");
	if (args.empty() || args[0].empty())
		return sendReply(cl, "411", "", "No recipient given (PRIVMSG)");
	if (args.size() < 2 || args[1].empty())
		return sendReply(cl, "412", "", "No text to send");

	std::string message = args[1];
	std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";
	std::vector<std::string> targets = split(args[0], ',');

	for (size_t i = 0; i < targets.size(); i++)
	{
		std::string &target = targets[i];
		if (target.empty())
			continue;

		std::string fullMsg = ":" + prefix + " PRIVMSG " + target + " :" + message + "\r\n";
		if (target[0] == '#' || target[0] == '&')
		{
			std::map<std::string, Channel *>::iterator it = channels.find(target);
			if (it == channels.end())
			{
				sendReply(cl, "403", target, "No such channel");
				continue;
			}
			Channel *channel = it->second;
			if (channel->users.find(cl) == channel->users.end())
			{
				sendReply(cl, "404", target, "Cannot send to channel");
				continue;
			}
			broadcastToChannel(channel, fullMsg, cl);
		}
		else
		{
			client *dest = NULL;
			for (std::map<int, client *>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2)
			{
				if (it2->second->get_nickname() == target)
				{
					dest = it2->second;
					break;
				}
			}
			if (!dest)
			{
				sendReply(cl, "401", target, "No such nick/channel");
				continue;
			}
			dest->send_data(fullMsg);
		}
	}
}
