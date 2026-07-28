#include "channel.hpp"

static bool isValidChannelName(const std::string &name)
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

void server::joinOneChannel(client *cl, const std::string &channelName, const std::string &key)
{
	if (!cl->is_authenticated_client() || !cl->is_registered_client() || !cl->welcom_to_server())
	{
		sendReply(cl, "451", "", "You have not registered");
		return;
	}
	if (!isValidChannelName(channelName))
	{
		sendReply(cl, "403", channelName, "No such channel");
		return;
	}
	std::cout << "channel: " << channelName << std::endl;
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
		channel->operators.insert(cl);
	}
	else
	{
		channel = channels[channelName];

		if (channel->inviteOnly && channel->invitedUsers.find(cl) == channel->invitedUsers.end())
		{
			sendReply(cl, "473", channelName, "Cannot join channel (+i)");
			return;
		}
		if (channel->keyEnabled && key != channel->password)
		{
			sendReply(cl, "475", channelName, "Cannot join channel (+k)");
			return;
		}
		if (channel->limitEnabled && channel->users.size() >= channel->userLimit)
		{
			sendReply(cl, "471", channelName, "Cannot join channel (+l)");
			return;
		}
		if (channel->users.find(cl) != channel->users.end())
			return;
		if (cl->get_nickname() == "Ircbot" && !channels.empty())
		{
			for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
			{
				it->second->users.insert(cl);
			}
		}
	}
	channel->users.insert(cl);
	channel->invitedUsers.erase(cl); // consume invite if it was used
	notifyJoin(cl, channel);
}

void server::join(client *cl, std::vector<std::string> args)
{
	std::vector<std::string> channelNames;
	std::vector<std::string> keys;

	if (args.empty())
	{
		sendReply(cl, "461", "JOIN", "Not enough parameters");
		return;
	}
	channelNames = split(args[0], ',');
	if (args.size() > 1)
		keys = split(args[1], ',');
	for (size_t i = 0; i < channelNames.size(); ++i)
	{
		std::string key = (i < keys.size()) ? keys[i] : "";
		joinOneChannel(cl, channelNames[i], key);
	}
}