*This project has been created as part of the 42 curriculum by \<login1\>, \<login2\>.*

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
./ircserv 6667 secret
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
nc -C 127.0.0.1 6667
PASS secret
NICK testnick
USER testuser 0 * :Test User
JOIN #hello
PRIVMSG #hello :Hello world!
```

### Partial data test (from subject)

```bash
nc -C 127.0.0.1 6667
# Type: PASS secret (use ctrl+D to send in parts)
```

### Cleanup

```bash
make clean    # remove object files
make fclean   # remove objects + binary
make re       # full rebuild
```

## Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://tools.ietf.org/html/rfc2812)
- [RFC 2813 — IRC Server Protocol](https://tools.ietf.org/html/rfc2813)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Modern IRC documentation](https://modern.ircdocs.horse/)
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html)

### AI Usage

AI (Claude) was used to assist with:
- Structuring the class architecture (Server, Client, Channel, CommandHandler)
- Understanding IRC numeric reply codes and their proper format
- Reviewing edge cases in the MODE command parser
- Drafting the README template

All generated code was reviewed, tested, and understood by the project authors before inclusion. No code was blindly copy-pasted — every part was verified against the RFC and tested with a real IRC client.
