#include "server.hpp"
server::server()
{
}

server::server(int hostname, int type, int protocole)
    : hostname(hostname), type(type), protocole(protocole), backlog(Maxclient_fd) {}
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

void server::start_server(int port, std::string password)
{
    this->port = port;
    this->password = password;
    socketfd = socket_creat(hostname, type, protocole);
    socket_add(&server_addr, port);
    server_bind(socketfd, &server_addr);
    int listeninig = server_listen(socketfd);
    waiting_client_responce(socketfd, &client_addr, client_addrlen, listeninig);//this logic just for one client 
    struct pollfd pollfds[Maxclient_fd];

    
    pollfds[0].fd = socketfd;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
    connect_multiple_client(pollfds,Maxclient_fd, 1);//this for multiple client use pool
    server_close(socketfd);
}
int server::check_passwword(int clinet_fd)
{
    char buff[Buffer_size];
    memset(buff, 0, Buffer_size);
    send(client_fd, "Password", 10, 0);
    int rec = recv(client_fd, buff, Buffer_size, 0);
    if(rec < 0)
        return 1;
    std::string recevid(buff, rec);
    recevid.erase(recevid.find_last_not_of("\r\n") + 1);
    return(recevid == this->password);
}


int server::socket_creat(int hostname, int type, int protocole)
{
    int socketfd;
    socketfd = socket(hostname, type, protocole);
    if (socketfd == -1)
    {
        std::cout << "can't creat a socket" << std::endl;
        exit(-1);
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
    memset(&(src->sin_zero), 0, sizeof(src));
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
        exit(1);
    }
    else
    {
        std::cout << "listinig successe";
        return n_listen;
    }
}
void server::waiting_client_responce(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
    //this for multiple client 
    int count = 0;
    // Maxclient = socketfd + 1;
    //i need message if any client connect (new client connect and client disconnect)

    while (true)
    {
        std::cout << "waiting for client..." << std::endl;
        //the client remot name as a host
        // char host[NI_MAXHOST];
        
        // //the server information that the client connect for ;
        // char serv[NI_MAXSERV];


        // int sockname = getnameinfo((sockaddr *)&client_addr, sizeof(client_addr), host, NI_MAXHOST, serv, NI_MAXSERV); // this tell u just the information abou the server like the ip and the port
        // if (sockname == 0)
        // {
        //     std::cout<< "connect on this port :" << serv << std::endl;
        // }

        int client_fd = server_accept(socketfd, client_addr, client_addrlen, listinign);                 // this use to know the client comme to connect to the server
        snd_recv(client_fd);
        close(client_fd);
    }
}

int server::server_accept(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
    int client_fd;
    client_fd = accept(socketfd, (sockaddr *)&client_addr, &client_addrlen);
    if (client_fd < 0 && check_passwword(client_fd))
    {
        std::cout << "error accepting client connection can't successe";
        close(socketfd);
        exit(1);
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
//we can use the sellect() to connect multiple client's (use inside the threads)

// int server::connect_multiple_client(struct pollfd *pollfds, int Maxclient_fd, nfds_t nfds, int listening)
// {
//     // Maxclient_fd = NUM_FDS; i don't know this what is do 
//     fd_set fd_creat;
//     FD_ZERO(&fd_creat); //this for clear the file descripter
//     FD_SET(listening, &fd_creat);
//     while (true)
//     {
//        fd_set nfds = fd_creat;
//         /* code */
//         // int pool_client = poll(struct poolfd *fds, nfds_t nfds, int timeout);
//         int sellect_client = select( Maxclient_fd + 1, &nfds, nullptr, nullptr, nullptr);
//         if (sellect_client == -1)
//         { 
//             std::cout <<"error in the pool the multiple client " << std::endl;
//             break;
//         }
//         for(size_t i = 0; i < sellect_client; i++)
//         {
//             int sock = nfds;
//             FD_ISSET(client_addr[i], &nfds);
//             if (sock == listining)
//             {
//                 //new client
//                 int new_client = accept(listining, nullptr, nullptr);
//                 if(check_passwword(new_client))
//                 {
//                     close(new_client);
//                 }
//                 //set this thread use 
//                 FD_SET(new_client, &fd_creat);
//                 std::cout << "the new client connect";
//                 send(new_client, "hello new client",16 , 0);


//             }
//             else
//             {
//                 char buff[Buffer_size];
//                 memset(buff, 0, 4096);
//                 int receive_massage = recv(sock, buff, 4096, 0);

//                 //client disconnectes if it's not resice nutiong
//                 if (receive_massage <= 0)
//                 {
//                     close(sock);
//                     FD_CLR(sock, &fd_creat);
//                     --i;
//                 }
//                 else
//                 {
//                     // send(client_fd, buff, receive_massage + 1, 0);
//                     //check all the client can ressive the message from the client
//                     for(int i =0; i < fd_creat.fd_count; i++)
//                     {
//                         int outSock = fd_creat.fd_count[i];
//                         if(outSock != listining && outSock != sock)
//                             send(outSock, buff, receive_massage, 0);
//                     }
//                 }
//             }
//         }

//     }
    
//     // we have * POLLIN , * POLLOUT, *POLLPRI, *POLLRDHUP, *POLLERR, *POLLHUP, *POLLNVAL

// }
//pool the mltiple client 
    // struct pollfd{
    //     int fd;
    //     short events;
    //     short revents;
    // }
int server::connect_multiple_client(struct pollfd *pollfds, int Maxclient_fd, nfds_t nfds)
{
    // Maxclient_fd = NUM_FDS; i don't know this what is do 
    fd_set fd_creat;
    pollfds-> fd = listining;
    pollfds-> events = POLLIN;
    pollfds->revents = 0;
    int numfds = 1;
    while (true)
    {
        /* code */
        // int pool_client = poll(struct poolfd *fds, nfds_t nfds, int timeout);
        int poll_client = poll(pollfds, nfds, -1);
        if (poll_client == -1)
        { 
            std::cout <<"error in the pool the multiple client " << std::endl;
            break;
        }
        for(size_t i = 0; i < nfds; i++)
        {
            int sock = pollfds[i].fd;
            if (pollfds[i].revents && POLL_IN)
            {
                //new client
                int new_client = accept(listining, nullptr, nullptr);
                if(check_passwword(new_client))
                {
                    close(new_client);
                }
                else
                {
                    //set the client 
                    pollfds[nfds].fd = new_client;
                    pollfds[nfds].events = POLL_IN;
                    nfds++;
                }
            }
            else
            {
                char buff[Buffer_size];
                memset(buff, 0, 4096);
            int receive_massage = recv(client_fd, buff, Buffer_size, 0);

                //client disconnectes if it's not resice nutiong
                if (receive_massage <= 0)
                {
                    close(sock);
                    --i;  
                }
                else
                {
                    send(sock, buff, receive_massage, 0);
                }
            }
        }

    }
    
    // we have * POLLIN , * POLLOUT, *POLLPRI, *POLLRDHUP, *POLLERR, *POLLHUP, *POLLNVAL

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
        send(client_fd, buff, receive_massage + 1, 0);
    }
}

int server::server_close(int fd)
{
    int n_close = close(fd);
    if (n_close < 0)
    {
        std::cout << "error of the close " << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "closing successe" << std::endl;
        return n_close;
    }
}
