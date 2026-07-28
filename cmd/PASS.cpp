#include "channel.hpp"

void server::pass(client *client, std::vector<std::string> args, std::string password)
{
	if (args.empty())
	{
		sendReply(client, "461", "PASS", "Not enough parameters");
		return;
	}
	if (password != args[0])
	{
		sendReply(client, "464", "", "Password incorrect");
		return;
	}

	client->set_password(args[0]);
	client->is_authenticated_client(true);
}