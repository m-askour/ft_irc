#include "channel.hpp"

static bool hasRealnameColon(const std::string &rawLine)
{
	std::string line = rawLine;
	while (!line.empty() && line[0] == ' ')
		line.erase(0, 1);

	size_t firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
		return false;

	std::string command = line.substr(0, firstSpace);
	if (command != "USER")
		return false;

	size_t pos = firstSpace + 1;
	for (int i = 0; i < 3; ++i)
	{
		while (pos < line.size() && line[pos] == ' ')
			++pos;
		if (pos >= line.size())
			return false;

		size_t end = line.find(' ', pos);
		if (end == std::string::npos)
			end = line.size();
		pos = end;
	}

	while (pos < line.size() && line[pos] == ' ')
		++pos;

	return pos < line.size() && line[pos] == ':';
}

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

	if (!hasRealnameColon(rawLine))
	{
		sendReply(client, "461", "USER", "Missing ':' before realname");
		return;
	}

	client->set_username(args[0]);
	client->set_realname(args[3]);

	std::cout << "User registered: " << client->get_username() << std::endl;
	std::cout << "Real name: " << client->get_realname() << std::endl;

	client->welcom_to_server(true);
	sendReply(client, "001", client->get_nickname(), "Welcome to the IRC network");
}