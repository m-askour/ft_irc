#ifndef SERVER_HPP
#define SERVER_HPP
#include <exception>
// prototipe
// Client connects
//         │
//         ▼
// receive_data()
//         │
//         ▼
// extractLine()
//         │
//         ▼
// PASS -----> set_password()
//         │
// NICK -----> set_nickname()
//         │
// USER -----> set_username()
//         │
//         ▼
// if (PASS && NICK && USER)
//         │
//         ▼
// registered = true
//         │
//         ▼
/*socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept,
htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa,
inet_ntop, send, recv, signal, sigaction,
sigemptyset, sigfillset, sigaddset, sigdelset,
sigismember, lseek, fstat, fcntl, poll (or
equivalent)*/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/cdefs.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include<map>
#include <algorithm>
#define Buffer_size 4096
#include "./client/client.hpp"
#include "./cmd/cmd.hpp"
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
        bool is_multiple;        // flag to track mode
        int backlog;// this is the nember of the client listen for 
        int client_count;
        //-------alaa part -----//
        std::map<int, client*> clients;
        std::map<std::string, Channel*> channels;
        
        //----------------------------------//
        socklen_t server_addrlen;
        socklen_t client_addrlen;
        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
        nfds_t nfds;
public:
    server();
    server(int hostname, int type, int protocole);
    server(const server &other);
    server &operator=(const server &other);
    ~server();

    //-----------------------------------the first command----------------------------//
    void pass(client *client, std::vector<std::string> args, std::string password);   
    void nick(client *client, std::vector<std::string> args);
    void user(client *client, std::vector<std::string> args);

    //---------------------------------------alaa parts ------------------------------//
    void join(client *client, std::vector<std::string> args);
    void mode(client *client, const std::vector<std::string> &args);
    //---------------------------new commands ------------------------------//
    void privmsg(client *client, const std::vector<std::string> &args);
    void kick(client *client, const std::vector<std::string> &args);
    void invite(client *client, const std::vector<std::string> &args);
    void topic(client *client, const std::vector<std::string> &args);
    //---------------------------command dispatch --------------------------//
    void handleCommand(client *client, const std::string &rawLine, int n);
    std::vector<std::string> parseParams(const std::string &line, std::string &command);
    //---------------------------join functions--------------------------------//
    void joinOneChannel(client *client, const std::string &channelName, const std::string &key);
    void broadcastToChannel(Channel *channel, const std::string &message, client *exclude);
    void sendReply(client *client, const std::string &code, const std::string &params, const std::string &message);
    void notifyJoin(client *client, Channel *channel);
    std::vector<std::string> split(const std::string &s, char delim);
    //---------------------------------mode function -----------------------//
    client *find_client(const std::string &nickname);
    //--------------------------------------------------------------------------//
    std::string getPassword() const;
    std::string getServername() const;
    //this have all the proccess of the server 
    void start_server(int port, std::string password);
    //itape 1 creat a socket
    /*socket,setsockopt, getsockname*/
    int socket_creat(int hostname, int type, int protocole);
    
    //identity socket addres
    void socket_add(struct sockaddr_in *src, int port);

    //check the client is send a connection
    //1-binding 
    //this to get all the information about the client like ip the hoset
    /*getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo, bind*/
    int server_bind(int socketfd, struct sockaddr_in *src);

    //2-listening
    //this use to check is there is any send of the conection from the client or no and wait in this connection 
    /* listen,*/
    int server_listen(int socketfd);


    //wating for requist 
    void waiting_client_responce(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign);
    int check_password(int client_fd);
    
    //// all about the client or as we call it in network the host
    /*gethostbyname API */ //(know all about the client and ) ## to know this is not for the work of the server but it's for the configuration and the flixebility 

    //3-accepte the client
    /* connect, accept*/
    int server_accept(int socketfd, struct sockaddr_in *client_addr, socklen_t client_addrlen, int listinign);

    //close the listingin
    int close_listining(int listining);

    //handle the send and recive 
    /*send, recv, signal*/
    void snd_recv(int client_fd);


    //handle multiple clients
    /*poll*/
    //manage connection new client connect/disconnect 

    int connect_multiple_client(struct pollfd *pollfds, nfds_t Maxclient_fd, nfds_t &nfds);
    //any connecion i need to put it in some where like stor it


    // int connect_multiple_client(struct pollfd *pollfds, nfds_t nfds);
    //4-close the server 
    /*close*/
    int server_close(int fd);
};



#endif
