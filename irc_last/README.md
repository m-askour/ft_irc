*This project has been created as part of the 42 curriculum by \msabr, \ahari, \maskour.*

# ft_irc — Internet Relay Chat Server

## Description

ft_irc is a fully functional IRC server written in **C++98**, built as part of the 42 school curriculum. The server implements the core IRC protocol and allows real IRC clients (such as irssi, HexChat, or LimeChat) to connect, authenticate, join channels, and exchange messages.

The server handles multiple simultaneous clients using a single non-blocking `poll()` event loop — no forking, no threads.

## Features

- Password-protected server access
- Client authentication (PASS / NICK / USER)
- Public channel messaging and private messaging (PRIVMSG / NOTICE)
- Channel creation and membership management (JOIN / PART)
- Channel operator commands: KICK, INVITE, TOPIC, MODE
- Supported channel modes: `i` (invite-only), `t` (topic lock), `k` (key/password), `o` (operator), `l` (user limit)
- Non-blocking I/O with a single `poll()` loop
- Graceful client disconnection and cleanup
- Partial message buffering (handles split TCP packets)

## Instructions

### Compilation

```bash
make
```

This produces the `ircserv` binary. Requires a C++98-compatible compiler (`c++` / `g++`).

### Running the server

```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 8080 secret
```

### Connecting with a client

Using **irssi**:
```
/connect 127.0.0.1 6667 secret
/nick mynick
/join #general
```

Using **nc** (for testing):
```bash
nc -C 127.0.0.1 8080
PASS secret
NICK testnick
USER testuser 0 * :Test User
JOIN #hello
PRIVMSG #hello :Hello world!
```

### Partial data test (from subject)

```bash
nc -C 127.0.0.1 8080
# Type: PASS secret (use ctrl+D to send in parts)
```

### Cleanup

```bash
make clean    # remove object files
make fclean   # remove objects + binary
make re       # full rebuild
```
### Bonus
```bash
make bonus    # make bonus
```
### Connect boot with server cmd
```bash
./ircbot <hostname> <port> <password>
```
**Example:**
```bash
./ircbot 127.0.0.1 8080 secret
```

### Cominicat with a client bot

Using **irssi**:
```
./ircbot 127.0.0.1 6667 secret
```

Using **nc** (for testing):
```bash
nc -C 127.0.0.1 8080
PASS secret
NICK testnick
USER testuser 0 * :Test User
JOIN #hello
PRIVMSG #hello :Hello world!
privmsg #chan :is this is a bot?
:Ircbot!Ircbot@localhost PRIVMSG #chan :Oh, good question... hmm... I don't know the answer
privmsg #chan :healp
:Ircbot!Ircbot@localhost PRIVMSG #chan :I'm not Google or ChatGPT... I actually think before answering
privmsg #chan :My name is moha 
:Ircbot!Ircbot@localhost PRIVMSG #chan :Nice to meet you, moha!
privmsg #chan :What's my name
:Ircbot!Ircbot@localhost PRIVMSG #chan :Your name is moha
```
## Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Modern IRC documentation](https://modern.ircdocs.horse/)
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html)
- [RFC bot](https://xsoneconsultants.com/blog/how-to-make-an-ai-chatbot-in-c/#Understanding_the_Basics_of_AI_Chatbots)
