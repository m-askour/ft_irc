#include "bot.hpp"

static int strToInt(const std::string &s)
{
	std::istringstream iss(s);
	int value = 0;
	iss >> value;
	if (iss.fail())
		throw std::invalid_argument("strToInt: not a number");
	return value;
}

static std::string intToStr(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bot::bot(const std::string &nick, const std::string &chan)
	: input(""), response(""), server_port(0),
	  botNick(nick), channel(chan), recvBuffer(""), registered(false)
{
	socketfd = -1;
}

bot::~bot()
{
	if (socketfd >= 0)
		close(socketfd);
}

void bot::sendLine(const std::string &line)
{
	std::string out = line;
	if (out.size() < 2 || out.substr(out.size() - 2) != "\r\n")
		out += "\r\n";
	send(socketfd, out.c_str(), out.length(), 0);
}

void bot::joinChannel(const std::string &channelName)
{
	sendLine("JOIN " + channelName);
}

void bot::sendMessage(const std::string &target, const std::string &text)
{
	sendLine("PRIVMSG " + target + " :" + text);
}

void bot::quitServer(const std::string &reason)
{
	sendLine("QUIT :" + reason);
}

bool bot::connectToServer(std::string ip, int port, std::string password)
{
	this->server_ip = ip;
	this->server_port = port;
	this->server_password = password;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		std::cout << "Error creating bot socket" << std::endl;
		return false;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "Error: bot could not connect to its own server" << std::endl;
		close(socketfd);
		socketfd = -1;
		return false;
	}

	std::cout << "[bot] connected to " << ip << ":" << port << std::endl;

	sendLine("PASS " + password);
	sendLine("NICK " + botNick);
	sendLine("USER " + botNick + " 0 * :" + botNick);

	return true;
}

void bot::handleServerLine(const std::string &rawLine)
{
	std::string line = rawLine;
	while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
		line.erase(line.size() - 1);
	if (line.empty())
		return;

	std::cout << "[bot] Server: " << line << std::endl;

	std::string prefix, command;
	std::vector<std::string> params = server::parseParams(line, command, &prefix);

	for (size_t i = 0; i < command.size(); ++i)
		command[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(command[i])));

	if (command == "PING")
	{
		std::string token = params.empty() ? "" : params[0];
		sendLine("PONG :" + token);
		return;
	}

	if (command == "001")
	{
		registered = true;
		joinChannel(channel);
		return;
	}

	if (command == "433")
	{
		botNick += "_";
		sendLine("NICK " + botNick);
		return;
	}

	if (command == "PRIVMSG")
	{
		handlePrivmsg(prefix, params);
		return;
	}
}

void bot::handlePrivmsg(const std::string &prefix, const std::vector<std::string> &params)
{
	if (params.size() < 2)
		return;

	std::string target = params[0];
	std::string text = params[1];

	std::string senderNick = prefix.substr(0, prefix.find('!'));
	if (senderNick == botNick)
		return;

	std::string replyTarget = (target == botNick) ? senderNick : target;

	std::string botReply = processinput(text);
	sendMessage(replyTarget, botReply);
	std::cout << "[bot] -> " << replyTarget << ": " << botReply << std::endl;
}

bool bot::handleLocalCommand(const std::string &userInput)
{
	if (userInput.empty())
		return false;

	if (userInput[0] != '/')
	{
		sendMessage(channel, userInput);
		return true;
	}

	std::string cmd = userInput.substr(1);
	size_t sp1 = cmd.find(' ');
	std::string verb = (sp1 == std::string::npos) ? cmd : cmd.substr(0, sp1);
	std::string rest = (sp1 == std::string::npos) ? "" : cmd.substr(sp1 + 1);

	if (verb == "join")
		joinChannel(rest);
	else if (verb == "quit" || verb == "exit")
		quitServer("leaving");
	else
		sendLine(rest.empty() ? verb : verb + " " + rest);

	return true;
}

void bot::communicateWithServer()
{
	char buff[1024];

	while (true)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(socketfd, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
		int maxfd = socketfd > STDIN_FILENO ? socketfd : STDIN_FILENO;

		int ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
		if (ready < 0)
		{
			std::cout << "select error" << std::endl;
			break;
		}

		if (FD_ISSET(socketfd, &readfds))
		{
			memset(buff, 0, sizeof(buff));
			int recvd = recv(socketfd, buff, sizeof(buff) - 1, 0);
			if (recvd <= 0)
			{
				std::cout << "Disconnected from server." << std::endl;
				break;
			}
			recvBuffer.append(buff, recvd);

			size_t pos;
			while ((pos = recvBuffer.find("\r\n")) != std::string::npos)
			{
				std::string line = recvBuffer.substr(0, pos);
				recvBuffer.erase(0, pos + 2);
				handleServerLine(line);
			}
		}

		if (FD_ISSET(STDIN_FILENO, &readfds))
		{
			std::string userInput;
			if (!std::getline(std::cin, userInput))
				break;

			if (userInput == "exit")
			{
				std::cout << "Disconnecting..." << std::endl;
				quitServer("leaving");
				break;
			}

			if (handleLocalCommand(userInput))
				continue;
		}
	}

	close(socketfd);
	socketfd = -1;
}

std::string bot::processinput(std::string &input)
{
	history.push_back(input);
	std::string response;
	if (keywordMatching(input, response))
		return response;
	return failbackResponse();
}

std::string calculate(std::string input)
{
	int pos_op = -1;
	char op = 0;

	for (size_t i = 0; i < input.size(); i++)
	{
		if (input[i] == '+' || input[i] == '-' ||
			input[i] == '*' || input[i] == '/')
		{
			pos_op = static_cast<int>(i);
			op = input[i];
			break;
		}
	}

	if (pos_op == -1)
		return "Invalid format";

	try
	{
		int num1 = strToInt(input.substr(0, pos_op));
		int num2 = strToInt(input.substr(pos_op + 1));
		int result = 0;

		switch (op)
		{
		case '+':
			result = num1 + num2;
			break;
		case '-':
			result = num1 - num2;
			break;
		case '*':
			result = num1 * num2;
			break;
		case '/':
			if (num2 != 0)
				result = num1 / num2;
			else
				return "Error: division by zero";
			break;
		default:
			return "Error";
		}

		return intToStr(result);
	}
	catch (...)
	{
		return "Invalid format";
	}
}

bool bot::keywordMatching(std::string &input, std::string &response)
{
	if (input.find('?') != std::string::npos)
	{
		response = "Oh, good question... hmm... I don't know the answer";
		return true;
	}
	else if (input.find("HI") != std::string::npos ||
			 input.find("Hi") != std::string::npos ||
			 input.find("hi") != std::string::npos ||
			 input.find("Hello") != std::string::npos ||
			 input.find("hello") != std::string::npos ||
			 input.find("hey") != std::string::npos)
	{
		response = "Hi";
		return true;
	}
	else if (input.find("help") != std::string::npos)
	{
		response = "Hi, I'm a chat bot. Try !topic, !kick, !invite, or ask me a question";
		return true;
	}
	else if (input.find("Say my name") != std::string::npos)
	{
		if (!userName.empty())
			response = "Your name is " + userName;
		else
			response = "What's your name? Tell me!";
		return true;
	}
	else if (input.find("My name is") != std::string::npos)
	{
		size_t pos = input.find("My name is") + 10;
		userName = input.substr(pos);
		while (!userName.empty() && userName[0] == ' ')
			userName.erase(0, 1);
		response = "Nice to meet you, " + userName + "!";
		return true;
	}
	else if (input.find("+") != std::string::npos || input.find("-") != std::string::npos ||
			 input.find("/") != std::string::npos || input.find("*") != std::string::npos)
	{
		response = calculate(input);
		return true;
	}
	else
	{
		response = "I'm not Google or ChatGPT... I actually think before answering";
		return true;
	}
}

std::string bot::failbackResponse()
{
	return "I don't understand what you mean";
}