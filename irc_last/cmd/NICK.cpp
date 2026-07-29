#include "channel.hpp"

bool is_valid_nickname(const std::string &nick)
{
	if (nick.empty() || nick.size() > 9)
		return false;
	if (isdigit(nick[0]) || nick[0] == '-')
		return false;
	for (size_t i = 0; i < nick.size(); i++)
	{
		if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_' &&
			nick[i] != '[' && nick[i] != ']' && nick[i] != '{' && nick[i] != '}' &&
			nick[i] != '\\' && nick[i] != '|')
			return false;
	}
	return true;
}
void server::nick(client *cl, std::vector<std::string> args)
{
	if (args.empty())
	{
		sendReply(cl, "431", "", "No nickname given");
		return;
	}
	if (!cl->is_authenticated_client())
	{
		sendReply(cl, "464", "", "Password required");
		return;
	}
	const std::string &newNick = args[0];
	if (!is_valid_nickname(newNick))
	{
		sendReply(cl, "432", newNick, "Erroneous nickname");
		return;
	}

	for (std::map<int, client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second == cl)
			continue;
		if (it->second->get_nickname() == newNick)
		{
			sendReply(cl, "433", newNick, "Nickname is already in use");
			return;
		}
	}
	cl->set_nickname(newNick);
	cl->is_registered_client(true);
}