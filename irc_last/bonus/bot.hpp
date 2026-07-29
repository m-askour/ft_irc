#ifndef BOT_HPP
#define BOT_HPP

#include <cstdlib>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>


#include "../server/server.hpp"

class server;
class bot
{
	private:
		std::string input;
		std::vector<std::string> history;
		std::string response;
		std::string userName;

		int socketfd;
		struct sockaddr_in server_addr;
		std::string server_ip;
		int server_port;
		std::string server_password;

		std::string botNick;
		std::string channel;
		std::string recvBuffer;
		bool registered;

		bot(const bot &other);
		bot &operator=(const bot &other);

		void sendLine(const std::string &line);
		void handleServerLine(const std::string &line);
		void handlePrivmsg(const std::string &prefix, const std::vector<std::string> &params);
		bool handleLocalCommand(const std::string &userInput);

	public:
		bot(const std::string &nick, const std::string &chan);
		~bot();

		bool connectToServer(std::string ip, int port, std::string password);
		void communicateWithServer();

		void joinChannel(const std::string &channelName);
		void sendMessage(const std::string &target, const std::string &text);
		void quitServer(const std::string &reason = "leaving");

		std::string processinput(std::string &input);
		bool keywordMatching(std::string &input, std::string &response);
		std::string failbackResponse();
};

std::string calculate(std::string input);

#endif