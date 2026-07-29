#include "channel.hpp"

void server::user(client *client, std::vector<std::string> args, const std::string &rawLine)
{
	if (args.size() < 4)
	{
		sendReply(client, "461", "USER", "Not enough parameters");
		return;
	}

	if (!client->is_authenticated_client())
	{
		sendReply(client, "464", "", "Password required");
		return;
	}

	if (!client->is_registered_client())
	{
		sendReply(client, "462", "", "You may not reregister");
		return;
	}
	if (args[0].empty())
	{
		sendReply(client, "461", "USER", "Not enough parameters");
		return;
	}
		std::string realname;
	size_t colonPos = rawLine.find(" :");
	if (colonPos != std::string::npos)
		realname = rawLine.substr(colonPos + 2);
	else
		realname = args[3]; 
	client->set_username(args[0]);
	client->set_realname(realname);

	std::cout << "User registered: " << client->get_username() << std::endl;
	std::cout << "Real name: " << client->get_realname() << std::endl;

	client->welcom_to_server(true);
	sendReply(client, "001", client->get_nickname(), "Welcome to the IRC network");
}