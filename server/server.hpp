#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#define Buffer_size 4096
#include "./client/client.hpp"
#include "./cmd/channel.hpp"

class server
{
	private:
		int ip;
		int hostname;
		int type;
		int protocole;
		int listining;
		std::string password;
		int socketfd;
		int port;
		int client_fd;
		nfds_t Maxclient_fd;
		bool is_multiple;
		int backlog;
		int client_count;
		//-------alaa part -----//
		std::map<int, client *> clients;
		std::map<std::string, Channel *> channels;
		//----------------------------------//
		socklen_t server_addrlen;
		socklen_t client_addrlen;
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		nfds_t nfds;

	public:
		server(int hostname, int type, int protocole);
		server(const server &other);
		server &operator=(const server &other);
		~server();
		//-----------------------------------the first command----------------------------//
		void pass(client *client, std::vector<std::string> args, std::string password);
		void nick(client *client, std::vector<std::string> args);
		void user(client *client, std::vector<std::string> args, const std::string &rawLine);
		//---------------------------------------alaa parts ------------------------------//
		void join(client *cl, std::vector<std::string> args);
		void mode(client *client, const std::vector<std::string> &args);
		//---------------------------new commands ------------------------------//
		void privmsg(client *client, const std::vector<std::string> &args);
		void kick(client *client, const std::vector<std::string> &args);
		void invite(client *client, const std::vector<std::string> &args);
		void topic(client *client, const std::vector<std::string> &args);
		//---------------------------command dispatch --------------------------//
		void handleCommand(client *client, const std::string &rawLine);
		static std::vector<std::string> parseParams(const std::string &line, std::string &command, std::string *prefix = NULL);
		//---------------------------join functions--------------------------------//
		void sendNamesList(client *cl, Channel *channel);
		void sendReply(client *cl, const std::string &code, const std::string &params, const std::string &message);
		void joinOneChannel(client *cl, const std::string &channelName, const std::string &key);
		void broadcastToChannel(Channel *channel, const std::string &message, client *exclude);
		void notifyJoin(client *cl, Channel *channel);
		std::vector<std::string> split(const std::string &s, char delim);
		//---------------------------------mode function -----------------------//
		client *find_client(const std::string &nickname);
		//--------------------------------------------------------------------------//
		std::string getPassword() const;
		std::string getServername() const;
		// this have all the proccess of the server
		void start_server(int port, std::string password);
		// itape 1 creat a socket
		/*socket,setsockopt, getsockname*/
		int socket_creat(int hostname, int type, int protocole);
		// identity socket addres
		void socket_add(struct sockaddr_in *src, int port);
		// check the client is send a connection
		// 1-binding
		int server_bind(int socketfd, struct sockaddr_in *src);
		// 2-listening
		/* listen,*/
		int server_listen(int socketfd);
		// 3-accepte the client
		/* connect, accept*/
		int server_accept(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign);
		// close the listingin
		int close_listining(int listining);
		/*poll*/
		// manage connection new client connect/disconnect
		int connect_multiple_client(struct pollfd *pollfds, nfds_t Maxclient_fd, nfds_t &nfds);
		// 4-close the server
		/*close*/
		int server_close(int fd);
};
