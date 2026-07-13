#include "server.hpp"
#include "./client/client.hpp"
#include <cctype>
server::server()
{
}

server::server(int hostname, int type, int protocole)
    : hostname(hostname),
      type(type),
      protocole(protocole),
      Maxclient_fd(1024),
      backlog(Maxclient_fd)
{
}
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
    // waiting_client_responce(socketfd, &client_addr, client_addrlen, listeninig);//this logic just for one client
    struct pollfd pollfds[Maxclient_fd];

    pollfds[0].fd = socketfd;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
    connect_multiple_client(pollfds, Maxclient_fd, this->nfds); // this for multiple client use pool
    server_close(socketfd);
}
int server::socket_creat(int hostname, int type, int protocole)
{
    int socketfd;
    socketfd = socket(hostname, type, protocole);
    if (socketfd == -1)
    {
        std::cout << "can't creat a socket" << std::endl;
        return (-1);
    }
    else
    {
        std::cout << "creat socket successe" << std::endl;
        return socketfd;
    }
}
/*struct sockaddr_in {
    __uint8_t       sin_len;
    sa_family_t     sin_family;
    in_port_t       sin_port;
    struct  in_addr sin_addr;
    char            sin_zero[8];
};*/
void server::socket_add(struct sockaddr_in *src, int port)
{
    src->sin_family = AF_INET;
    src->sin_port = htons(port);
    src->sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(src->sin_zero, 0, sizeof(src->sin_zero));
}

int server::server_bind(int socketfd, struct sockaddr_in *src)
{
    int n_bind = bind(socketfd, (struct sockaddr *)src, sizeof(*src));
    if (n_bind < 0)
    {
        std::cout << "error binding can't successe" << std::endl;
        return (-1);
    }
    else
    {

        /// desply it's connect the client
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
        return (-1);
    }
    else
    {
        std::cout << "listinig successe";
        return n_listen;
    }
}
void server::waiting_client_responce(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
    // this for multiple client
    //  int count = 0;

    // Maxclient = socketfd + 1;
    // i need message if any client connect (new client connect and client disconnect)

    while (true)
    {
        std::cout << "waiting for client..." << std::endl;
        // the client remot name as a host
        //  char host[NI_MAXHOST];

        // //the server information that the client connect for ;
        // char serv[NI_MAXSERV];

        // int sockname = getnameinfo((sockaddr *)&client_addr, sizeof(client_addr), host, NI_MAXHOST, serv, NI_MAXSERV); // this tell u just the information abou the server like the ip and the port
        // if (sockname == 0)
        // {
        //     std::cout<< "connect on this port :" << serv << std::endl;
        // }

        int client_fd = server_accept(socketfd, client_addr, client_addrlen, listinign); // this use to know the client comme to connect to the server
        snd_recv(client_fd);
        close(client_fd);
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
        // close_listining(listinign);
        return client_fd;
    }
}

int server::close_listining(int listining)
{
    close(listining);
    return 0;
}
// we can use the sellect() to connect multiple client's (use inside the threads)
/*
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
                clients[new_client] = client(new_client);
                if (nfds < Maxclient_fd)
                {
                    pollfds[nfds].fd = new_client;
                    pollfds[nfds].events = POLLIN;
                    nfds++;
                }
                else
                {
                    close(new_client);
                }
            }
            else if (pollfds[i].revents & POLLIN)
            {
                char buff[Buffer_size];
                memset(buff, 0, Buffer_size);

                client &cl = clients[pollfds[i].fd];

                int received = cl.receive_data();
                if (received <= 0)
                {
                    close(pollfds[i].fd);
                    clients.erase(pollfds[i].fd);
                    pollfds[i].fd = -1;
                }
                else
                {
                    send(pollfds[i].fd, buff, received, 0);
                }
            }
        }
    }

    return 0;
}*/
// ----- i change this to be aply to work on my code can mochikl fi data kifach tt presenta 3tit chat i kad hadchi o sf ra khalit lk dyalk lfo9---------------------//
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

        
                /*--------------i add new- -------------*/
                clients[new_client] = new client(new_client);
                if(clients[new_client]->check_client_info() == 0)
                {
                    std::string line = clients[new_client]->extractLine();
                    handleCommand(clients[new_client], line, 1);
                    clients[new_client]->print_client_info();
                }

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
                /****************here */
                std::map<int, client*>::iterator it = clients.find(pollfds[i].fd);
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
                        handleCommand(cl, line, 0);
                    }
                }
            }
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

void server::snd_recv(int client_fd)
{
    char buff[Buffer_size];
    while (true)
    {
        // clear the baffet
        memset(buff, 0, 4096);

        // receive or waiting for the massage
        int receive_massage = recv(client_fd, buff, 4096, 0);
        // error of the recive  message == -1
        if (receive_massage == -1)
        {
            std::cout << "error of receive the message of the client" << std::endl;
            break;
        }
        // client disconnect message == 0
        if (receive_massage == 0)
        {
            std::cout << " the client is disconnet " << std::endl;
            break;
        }
        // desplay the massage
        std::cout << "Received:" << std::string(buff, 0, receive_massage) << std::endl;
        // send the message
        send(client_fd, buff, receive_massage, 0);
    }
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

// - --- --- -- --- -haya mohammed hadi dyal replay -------- --------//
void server::sendReply(client *client, const std::string &code, const std::string &params, const std::string &message)
{
    std::string reply = ":" + this->getServername() + " " + code + " " + client->get_nickname();
    if (!params.empty())
        reply += " " + params;
    reply += " :" + message + "\r\n";
    client->send_data(reply); // or send() directly on client->fd
}
/*---------------------- o hadi dyal broadcast--------------*/
void server::broadcastToChannel(Channel *channel, const std::string &message, client *exclude)
{
    for (std::set<client*>::iterator it = channel->users.begin(); it != channel->users.end(); ++it)
    {
        if (*it != exclude)
            (*it)->send_data(message);
    }
}
/*--------------hadi dyali -------------------------------------------*/
void server::notifyJoin(client *client, Channel *channel)
{
    std::cout << channel->name << std::endl;
    std::string joinMsg = ":" + client->get_nickname() + " JOIN :" + channel->name + "\r\n";

    client->send_data(joinMsg);
    broadcastToChannel(channel, joinMsg, client);
}

//---------------------search client using nikename-----------//

client *server::find_client(const std::string &nickname)
{
    for (std::map<int, client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->get_nickname() == nickname)
            return new client(*it->second);
    }
    return NULL;
}

//---------------------- IRC line parsing -----------------------//
// Splits a raw IRC line into a command name and a parameter vector,
// following the standard grammar: an optional ":<prefix>" is skipped,
// the next token is the command, remaining tokens are space separated
// params except the last one may start with ':' and contain spaces.
std::vector<std::string> server::parseParams(const std::string &line, std::string &command)
{
    std::vector<std::string> params;
    size_t pos = 0;
    size_t len = line.size();

    command.clear();

    if (pos < len && line[pos] == ':')
    {
        size_t sp = line.find(' ', pos);
        if (sp == std::string::npos)
            return params;
        pos = sp + 1;
    }

    while (pos < len && line[pos] == ' ')
        pos++;

    size_t sp = line.find(' ', pos);
    if (sp == std::string::npos)
    {
        command = line.substr(pos);
        return params;
    }
    command = line.substr(pos, sp - pos);
    pos = sp + 1;

    while (pos < len)
    {
        while (pos < len && line[pos] == ' ')
            pos++;
        if (pos >= len)
            break;
        if (line[pos] == ':')
        {
            params.push_back(line.substr(pos + 1));
            break;
        }
        size_t next = line.find(' ', pos);
        if (next == std::string::npos)
        {
            params.push_back(line.substr(pos));
            break;
        }
        params.push_back(line.substr(pos, next - pos));
        pos = next + 1;
    }
    return params;
}

//---------------------- command dispatcher -----------------------//
void server::handleCommand(client *cl, const std::string &rawLine, int n)
{
    std::string line = rawLine;

    while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
        line.erase(line.size() - 1);
    if (line.empty() || cl == NULL)
        return;

    std::string command;
    std::vector<std::string> params = parseParams(line, command);

    for (size_t i = 0; i < command.size(); ++i)
        command[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(command[i])));
    if(n == 1)
    {
        if(command == "PASS")
            pass(cl, params, this->password);
        else if(command == "NICK")
            nick(cl, params);
        else if (command=="USER")
            user(cl, params);
        sendReply(cl, "421", command, "Unknown command");

    }
    else if(n == 0)
    {    
        if (command.empty())
            return;
        else if (command == "JOIN")
            join(cl, params);
        else if (command == "MODE")
            mode(cl, params);
        else if (command == "PRIVMSG")
            privmsg(cl, params);
        else if (command == "KICK")
            kick(cl, params);
        else if (command == "INVITE")
            invite(cl, params);
        else if (command == "TOPIC")
            topic(cl, params);
        else
            sendReply(cl, "421", command, "Unknown command");
    }
}