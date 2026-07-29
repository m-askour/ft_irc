#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
class client
{
	private:
		int client_fd;

		std::string password;
		std::string username;
		std::string nickname;
		std::string input_buff;
		std::string output_buff;
		std::string realname;
		std::string client_ip;
		bool is_authenticated;
		bool is_registered;
		bool all_done;

	public:
		client();
		client(int fd);
		client(const client &other);
		~client();
		void set_password(const std::string &pwd);
		void set_username(const std::string &user);
		void set_nickname(const std::string &nick);
		void set_realname(const std::string &realname);
		void set_client_ip(struct sockaddr_in client_addr);
		void is_authenticated_client(bool value);
		bool is_authenticated_client() const;
		void is_registered_client(bool value);
		bool is_registered_client() const;
		bool welcom_to_server() const;
		void welcom_to_server(bool value);
		int get_fd() const;
		const std::string &get_username() const;
		const std::string &get_nickname() const;
		const std::string &get_password() const;
		const std::string &get_realname() const;
		const std::string &get_buffer() const;
		std::string &get_client_ip();

		bool sendBufferEmpty() const;
		void send_data(const std::string &data);
		void flushSend();
		ssize_t receive_data();
		std::string extractLine();
		bool hasCompleteLine();
};

#endif