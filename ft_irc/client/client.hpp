#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../server/server.hpp"

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