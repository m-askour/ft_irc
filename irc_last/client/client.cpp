#include "client.hpp"

client::client()
    : client_fd(-1),
      is_authenticated(false),
      is_registered(false)
{
}

client::client(int fd)
    : client_fd(fd),
      is_authenticated(false),
      is_registered(false)
{
}
client::~client()
{
}
client::client(const client &other)
    : client_fd(other.client_fd),
      password(other.password),
      username(other.username),
      nickname(other.nickname),
      input_buff(other.input_buff),
      output_buff(other.output_buff),
      is_authenticated(other.is_authenticated),
      is_registered(other.is_registered), all_done(other.all_done)
{
}
void client::set_password(const std::string &pwd)
{
    password = pwd;
}
void client::set_username(const std::string &user)
{
    username = user;
}
void client::set_nickname(const std::string &nick)
{
    nickname = nick;
}
void client::set_realname(const std::string &realname)
{
    this->realname = realname;
}

void client::set_client_ip(struct sockaddr_in client_addr)
{
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, buf, sizeof(buf));
    std::cout << &client_addr << std::endl;
    client_ip = buf;
}

void client::is_authenticated_client(bool value)
{
    is_authenticated = value;
}
bool client::is_authenticated_client() const
{
    return is_authenticated;
}
void client::is_registered_client(bool value)
{
    is_registered = value;
}

bool client::is_registered_client() const
{
    return is_registered;
}
bool client::welcom_to_server() const
{
    return all_done;
}
void client::welcom_to_server(bool value)
{
    all_done = value;
}

int client::get_fd() const
{
    return client_fd;
}
const std::string &client::get_username() const
{
    return this->username;
}

const std::string &client::get_nickname() const
{
    return this->nickname;
}
const std::string &client::get_password() const
{
    return password;
}
const std::string &client::get_buffer() const
{
    return input_buff;
}
const std::string &client::get_realname() const
{
    return this->realname;
}

std::string &client::get_client_ip()
{
    return client_ip;
}

bool client::sendBufferEmpty() const
{
    return output_buff.empty();
}
void client::flushSend()
{
    if (output_buff.empty())
        return;

    ssize_t n = send(client_fd,
                     output_buff.c_str(),
                     output_buff.size(),
                     0);

    if (n > 0)
        output_buff.erase(0, n);
}
void client::send_data(const std::string &data)
{
    output_buff += data;
}
ssize_t client::receive_data()
{
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';
        input_buff += buffer;
    }
    return bytes_received;
}
std::string client::extractLine()
{
    size_t pos = input_buff.find('\n');
    if (pos == std::string::npos)
        return "";
    std::string line = input_buff.substr(0, pos);
    input_buff.erase(0, pos + 1);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    return line;
}
bool client::hasCompleteLine()
{
    return input_buff.find("\n") != std::string::npos;
}
