#include "channel.hpp"

std::vector<std::string> server::split(const std::string &s, char delim)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream stream(s);

	while (std::getline(stream, token, delim))
		tokens.push_back(token);

	return tokens;
}

void server::sendReply(client *cl, const std::string &code, const std::string &params, const std::string &message)
{
	std::string reply = ":" + this->getServername() + " " + code + " " + cl->get_nickname();
	if (!params.empty())
		reply += " " + params;
	reply += " :" + message + "\r\n";
	cl->send_data(reply);
}

void server::broadcastToChannel(Channel *channel, const std::string &message, client *exclude)
{
	for (std::set<client *>::iterator it = channel->users.begin(); it != channel->users.end(); ++it)
	{
		if (*it != exclude)
			(*it)->send_data(message);
	}
}

void server::notifyJoin(client *cl, Channel *channel)
{
	std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";

	std::string joinMsg = ":" + prefix + " JOIN :" + channel->name + "\r\n";

	cl->send_data(joinMsg);
	broadcastToChannel(channel, joinMsg, cl);

	sendNamesList(cl, channel);
}

void server::sendNamesList(client *cl, Channel *channel)
{
	if (!channel->topic.empty())
	{
		std::string topicMsg = ":" + getServername() + " 332 " + cl->get_nickname() + " " + channel->name + " :" + channel->topic + "\r\n";
		cl->send_data(topicMsg);
	}

	std::string names;
	for (std::set<client *>::iterator it = channel->users.begin(); it != channel->users.end(); ++it)
	{
		if (channel->operators.find(*it) != channel->operators.end())
			names += "@";
		names += (*it)->get_nickname() + " ";
	}
	std::string namesMsg = ":" + getServername() + " 353 " + cl->get_nickname() + " = " + channel->name + " :" + names + "\r\n";
	cl->send_data(namesMsg);

	std::string endMsg = ":" + getServername() + " 366 " + cl->get_nickname() + " " + channel->name + " :End of /NAMES list.\r\n";
	cl->send_data(endMsg);
}
client *server::find_client(const std::string &nickname) // ircbot
{
	for (std::map<int, client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->get_nickname() == nickname)
			return it->second;
	}
	return NULL;
}