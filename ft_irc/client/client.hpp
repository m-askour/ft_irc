#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../server/server.hpp"
// ##1.2 Clients

//    A client is anything connecting to a server that is not another
//    server.  Each client is distinguished from other clients by a unique
//    nickname having a maximum length of nine (9) characters.  See the
//    protocol grammar rules for what may and may not be used in a
//    nickname.  In addition to the nickname, all servers must have the
//    following information about all clients: the real name of the host
//    that the client is running on, the username of the client on that
//    host, and the server to which the client is connected.

// 1.2.1 Operators

//    To allow a reasonable amount of order to be kept within the IRC
//    network, a special class of clients (operators) is allowed to perform
//    general maintenance functions on the network.  Although the powers
//    granted to an operator can be considered as 'dangerous', they are
//    nonetheless required.  Operators should be able to perform basic
//    network tasks such as disconnecting and reconnecting servers as
//    needed to prevent long-term use of bad network routing.  In
//    recognition of this need, the protocol discussed herein provides for
//    operators only to be able to perform such functions.  See sections
//    4.1.7 (SQUIT) and 4.3.5 (CONNECT).

//    A more controversial power of operators is the ability  to  remove  a
//    user  from  the connected network by 'force', i.e. operators are able
//    to close the connection between any client and server.   The
//    justification for  this  is delicate since its abuse is both
//    destructive and annoying.  For further details on this type of
//    action, see section 4.6.1 (KILL).

class client : public server
{
private:
    int socketfd;
    struct sockaddr_in server_addr;
    socklen_t server_addrlen;
    std::string buffer;
    std::string password;
    std::string username;
    std::string nickname;
    bool is_authenticated;
    bool is_connected;
    bool is_registered;
    bool is_nickname_set;
    bool is_username_set;
    bool is_password_set;
    std::string server_ip;
    int server_port;

public:
    client();
    client(const std::string &server_ip, int server_port, const std::string &password = "");
    ~client();

    // configuration
    void set_server_ip(const std::string &ip);
    void set_server_port(int port);
    void set_password(const std::string &pwd);
    void set_username(const std::string &user);
    void set_nickname(const std::string &nick);

    // status checks
    bool is_authenticated_client() const;
    bool is_connected_client() const;
    bool is_registered_client() const;
    bool is_nickname_set_client() const;
    bool is_username_set_client() const;
    bool is_password_set_client() const;

    // getters
    int get_socketfd() const;
    const std::string &get_server_ip() const;
    int get_server_port() const;
    const std::string &get_username() const;
    const std::string &get_nickname() const;
    const std::string &get_password() const;
    const std::string &get_buffer() const;

    // operations
    bool connect_server();
    ssize_t send_data(const std::string &data);
    ssize_t receive_data();
    bool check_client_info() const;
    void print_client_info() const;
    void reset_client();
};

#endif
