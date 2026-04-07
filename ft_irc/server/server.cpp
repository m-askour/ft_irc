#include "server.hpp"
server::server()
{

}

server::server(int hostname, int type, int protocole)
    : hostname(hostname), type(type), protocole(protocole), backlog(5) {}
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
{}



void server::start_server(int port, std::string password)
{
    this->port = port;
    this->password = password;
    socketfd = socket_creat(hostname, type, protocole);
    socket_add(&server_addr, port);
    server_bind(socketfd, &server_addr);
    int listeninig = server_listen(socketfd);
    waiting_client_responce(socketfd, &client_addr, client_addrlen, listeninig);
    server_close(socketfd);
}



int server::socket_creat(int hostname, int type, int protocole)
{
    int socketfd;
    socketfd  = socket(hostname, type, protocole);
    if (socketfd == -1)
    {
        std::cout << "can't creat a socket" << std::endl;
        exit (-1);
    }
    else 
    {
        std::cout << "creat socket successe" <<std::endl;
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
    memset(&(src->sin_zero), 0, 8);
}

int server::server_bind(int socketfd, struct sockaddr_in *src)
{
    int n_bind = bind(socketfd, (struct sockaddr *)src, sizeof(*src));
    if(n_bind < 0)
    {
        std::cout << "error binding can't successe" <<std::endl;
        exit (1);
    }
    else
    {

        ///desply it's connect the client
        std::cout<<"binding successe"<<std::endl;
        return n_bind;
    }
}

int server::server_listen(int socketfd)
{
    int n_listen = listen(socketfd, backlog);
    if (n_listen < 0)
    {
        std::cout<<"error listitnig can't successe";
        exit(1);
    }
    else{
        std::cout<< "listinig successe";
        return n_listen;
    }
}
void server::waiting_client_responce(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
    while (true)
    {
        std::cout<<"waiting for client..."<<std::endl;
        server_accept(socketfd, client_addr, client_addrlen, listinign);
        snd_recv(client_fd);
        close(client_fd);
    }
}
int server::server_accept(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign)
{
    int client_fd;
    client_fd = accept(socketfd, (sockaddr*) &client_addr, &client_addrlen);
    if (client_fd < 0)
    {
        std::cout<<"error accepting client connection can't successe";
        close(socketfd);
        exit(1);
    }
    else
    {
        std::cout<<"accepting client connection successe";
        close_listining(listinign);
        return client_fd;
    }
}

int server::close_listining(int listining)
{
    close(listining);
    return 0;
}

int server::snd_recv(int client_fd)
{
    char buff[Buffer_size];
    while(true)
    {
            //clear the baffet
            memset(buff, 0, 4096);

            //receive or waiting for the massage 
            int receive_massage = recv(client_fd, buff, 4096,0 );
            //error of the recive  message == -1
            if (receive_massage == -1)
            {
                std::cout << "error of receive the message of the client" << std::endl;
                break;
            }
            //client disconnect message == 0
            if (receive_massage == 0)
            {
                std::cout << " the client is disconnet " << std::endl;
                break;
            }
            //desplay the massage
            std::cout << "Received:" << std::string(buff, 0, receive_massage) << std::endl;
            //send the message 
            send(client_fd, buff, receive_massage + 1, 0); 
    } 
}

int server::server_close(int fd)
{
    int n_close = close(fd);
    if (n_close < 0)
    {
        std::cout <<"error of the close " << std::endl;
        exit(1);
    }
    else
    {
        std::cout<<"closing successe" << std::endl;
        return n_close;
    }

}
