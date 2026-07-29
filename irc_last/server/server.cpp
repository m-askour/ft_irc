#include "server.hpp"

server::server(int hostname, int type, int protocole)
	: hostname(hostname),
	  type(type),
	  protocole(protocole),
	  Maxclient_fd(1024),
	  backlog(Maxclient_fd)
{}
server::server(const server &other)
{
	this->hostname = other.hostname;
	this->type = other.type;
	this->protocole = other.protocole;
}
server &server::operator=(const server &other)
{
	if (this != &other)
	{
		this->hostname = other.hostname;
		this->type = other.type;
		this->protocole = other.protocole;
	}
	return *this;
}

server::~server()
{
	for (std::map<int, client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second != NULL)
		{
			if (it->second->get_fd() >= 0)
				close(it->second->get_fd());
			delete it->second;
		}
	}
	clients.clear();

	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		delete it->second;
	}
	channels.clear();

	if (socketfd >= 0)
		close(socketfd);
}
std::string server::getPassword() const
{
	return this->password;
}
std::string server::getServername() const
{
	return "ircserv";
}
void server::start_server(int port, std::string password)
{
	this->port = port;
	this->password = password;
	socketfd = socket_creat(hostname, type, protocole);
	socket_add(&server_addr, port);
	server_bind(socketfd, &server_addr);
	int listeninig = server_listen(socketfd);
	this->listining = listeninig;
	struct pollfd pollfds[Maxclient_fd];

	pollfds[0].fd = socketfd;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	connect_multiple_client(pollfds, Maxclient_fd, this->nfds);
	server_close(socketfd);
}

int server::socket_creat(int hostname, int type, int protocole)
{
	int socketfd;
	socketfd = socket(hostname, type, protocole);
	if (socketfd == -1)
	{
		std::cout << "can't create a socket" << std::endl;
		return (-1);
	}
	else
	{
		int opt = 1;
		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cout << "setsockopt failed" << std::endl;
			close(socketfd);
			return (-1);
		}
		fcntl(socketfd, F_SETFL, O_NONBLOCK);
		std::cout << "create socket success" << std::endl;
		return socketfd;
	}
}

void server::socket_add(struct sockaddr_in *src, int port)
{
	src->sin_family = AF_INET;
	src->sin_port = htons(port);
	src->sin_addr.s_addr =INADDR_ANY;

	memset(src->sin_zero, 0, sizeof(src->sin_zero));
}

int server::server_bind(int socketfd, struct sockaddr_in *src)
{
	int n_bind = bind(socketfd, (struct sockaddr *)src, sizeof(*src));
	if (n_bind < 0)
	{
		std::cout << "error binding can't successe" << std::endl;
		exit(1);
	}
	else
	{
		std::cout << "binding successe" << std::endl;
		return n_bind;
	}
}

int server::server_listen(int socketfd)
{
	int n_listen = listen(socketfd, backlog);
	if (n_listen < 0)
	{
		std::cout << "error listitnig can't successe";
		exit(1);
	}
	else
	{
		std::cout << "listinig successe";
		return n_listen;
	}
}

int server::server_accept(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
	int client_fd;
	(void)listinign;
	client_fd = accept(socketfd, (sockaddr *)&client_addr, &client_addrlen);
	if (client_fd < 0)
	{
		std::cout << "error accepting client connection can't successe";
		close(socketfd);
		return (-1);
	}
	else
	{
		std::cout << "accepting client connection successe";
		return client_fd;
	}
}

int server::close_listining(int listining)
{
	close(listining);
	return 0;
}

void server::removeClientFromAllChannels(client *cl)
{
    for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); )
    {
        Channel *channel = it->second;
        if (channel->users.find(cl) != channel->users.end())
        {
            std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@" + cl->get_client_ip();
            std::string quitMsg = ":" + prefix + " QUIT :Client disconnected\r\n";
            broadcastToChannel(channel, quitMsg, cl);

            channel->users.erase(cl);
            channel->operators.erase(cl);
            channel->invitedUsers.erase(cl);

            if (channel->users.empty())
            {
                delete channel;
                channels.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

int server::connect_multiple_client(struct pollfd *pollfds, nfds_t Maxclient_fd, nfds_t &nfds)
{
	nfds = 1;
	pollfds[0].fd = socketfd;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;

	while (true)
	{
		int poll_client = poll(pollfds, nfds, -1);

		if (poll_client == -1)
		{
			std::cout << "poll error" << std::endl;
			break;
		}

		for (nfds_t i = 0; i < nfds; ++i)
		{
			std::map<int, client *>::iterator it = clients.find(pollfds[i].fd);
			if (pollfds[i].fd < 0)
				continue;

			if (pollfds[i].fd == socketfd)
			{
				if (!(pollfds[i].revents & POLLIN))
					continue;

				client_addrlen = sizeof(client_addr);
				int new_client = accept(socketfd, (struct sockaddr *)&client_addr, &client_addrlen);
				if (new_client < 0)
					continue;
				fcntl(new_client, F_SETFL, O_NONBLOCK);

				clients[new_client] = new client(new_client);
				clients[new_client]->set_client_ip(client_addr);
				if (nfds < Maxclient_fd)
				{
					pollfds[nfds].fd = new_client;
					pollfds[nfds].events = POLLIN;
					pollfds[nfds].revents = 0;
					nfds++;
					std::cout << "new client connected: fd " << new_client << std::endl;
				}
				else
				{
					std::cout << "server full, rejecting client fd " << new_client << std::endl;
					delete clients[new_client];
					clients.erase(new_client);
					close(new_client);
				}
			}
			else if (pollfds[i].revents & POLLIN)
			{
				if (it == clients.end() || it->second == NULL)
				{
					pollfds[i].fd = -1;
					continue;
				}
				client *cl = it->second;
				int received = cl->receive_data();

				if (received <= 0)
				{
					std::cout << "client disconnected: fd " << pollfds[i].fd << std::endl;
					removeClientFromAllChannels(cl);
					close(pollfds[i].fd);
					delete cl;
					clients.erase(pollfds[i].fd);
					pollfds[i].fd = -1;
				}
				else
				{
					while (cl->hasCompleteLine())
					{
						std::string line = cl->extractLine();
						std::cout << "recv from fd " << pollfds[i].fd << ": " << line << std::endl;
						handleCommand(cl, line);
					}
				}
			}
			else if (pollfds[i].revents & POLLOUT)
			{
				if (it == clients.end() || it->second == NULL)
				{
					pollfds[i].fd = -1;
					continue;
				}
				client *cl = it->second;
				cl->flushSend();
				if (cl->sendBufferEmpty())
				{
					pollfds[i].events &= ~POLLOUT;
				}
			}
		}
		for (nfds_t i = 0; i < nfds; ++i)
		{
			if (pollfds[i].fd < 0 || pollfds[i].fd == socketfd)
				continue;

			std::map<int, client *>::iterator it2 = clients.find(pollfds[i].fd);
			if (it2 == clients.end() || it2->second == NULL)
				continue;

			client *cl = it2->second;
			if (!cl->sendBufferEmpty())
				pollfds[i].events |= POLLOUT;
			else
				pollfds[i].events &= ~POLLOUT;
		}

		nfds_t writeIdx = 0;
		for (nfds_t readIdx = 0; readIdx < nfds; ++readIdx)
		{
			if (pollfds[readIdx].fd != -1)
			{
				if (writeIdx != readIdx)
					pollfds[writeIdx] = pollfds[readIdx];
				writeIdx++;
			}
		}
		nfds = writeIdx;
	}

	return 0;
}

int server::server_close(int fd)
{
	int n_close = close(fd);
	if (n_close < 0)
	{
		std::cout << "error of the close " << std::endl;
		return (-1);
	}
	else
	{
		std::cout << "closing successe" << std::endl;
		return n_close;
	}
}
