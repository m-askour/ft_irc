#ifndef CMD_HPP
#define CMD_HPP

#include "../client/client.hpp"

#include <iostream>
#include <sstream>
#include <set>

class Channel
{
	public:
		std::string name;
		std::string topic;
		std::string password;
		bool inviteOnly;
		bool topicProtected;
		bool keyEnabled;
		bool limitEnabled;
		std::set<client *> operators;
		std::set<client *> users;
		std::set<client *> invitedUsers;
		size_t userLimit;
};

#include "./server/server.hpp"

#endif
