/ircserv
 ├── main.cpp
 ├── Server.cpp / Server.hpp       (Person 1)
 ├── Parser.cpp / Parser.hpp       (Person 2)
 ├── Channel.cpp / Channel.hpp     (Person 3)
 ├── Commands/
 ├── Makefile

🔴 1. Duplicate constructor (COMPILATION ERROR)

You defined this twice:

server::server(int hostname, int type, int protocole)

➡️ This will give:

redefinition of ‘server::server(...)’
🔴 2. Wrong struct type (socketadd)

In header:

int server_bind(int socketfd, struct socketadd *src);

➡️ socketadd does NOT exist

✔️ You meant:

struct sockaddr_in
🔴 3. Wrong constant AF_INIT
src.sin_family = AF_INIT;

➡️ ❌ AF_INIT does not exist

✔️ Should be:

AF_INET
🔴 4. Passing struct by value (LOGIC BUG ⚠️)
void socket_add(struct sockaddr_in src, int port)

➡️ You modify a copy, NOT the real server_addr

👉 Result: your address stays uninitialized → bind fails

🔴 5. Wrong bind usage (VERY BIG BUG 🚨)
bind(socketfd, (sockaddr*) &src, sizeof src_len);

Problems:

&src → pointer to pointer ❌
src_len → DOES NOT EXIST ❌

👉 This alone will break everything

🔴 6. Wrong accept usage (CRITICAL BUG 🚨)
accept(socketfd, (sockaddr*) &client_addr, &client_addrlen);

❌ &client_addr is wrong (double pointer)

✔️ Should be:

(sockaddr*) client_addr
🔴 7. buff is NOT a buffer (COMPILATION ERROR)
#define buff 4096

Then:

memset(buff, 0, 4096);

➡️ You're using 4096 as if it's an array 😅

👉 This will NOT compile

🔴 8. Typo: clsoe
int c_close = clsoe(client_fd);

➡️ ❌ function does not exist

🔴 9. Missing return (UNDEFINED BEHAVIOR)
int close_listining(int listining)
{
    close(listining);
}

➡️ no return → UB

Same problem in:

int snd_recv(int client_fd)
🔴 10. Local variable hides member (LOGIC BUG)
int socketfd = socket_creat(...);

➡️ You are NOT setting:

this->socketfd

👉 later your class member is useless

🔴 11. Wrong function call (COMPILATION ERROR)
server_listen(socketfd, backlog);

But:

int server_listen(int socketfd);

➡️ extra argument → compile error

🔴 12. Wrong parameter mismatch
socket_add(&addr, port);

But function:

void socket_add(struct sockaddr_in src, int port);

➡️ pointer vs value mismatch

🔴 13. struct socketadd *src used again
int server_bind(int socketfd, struct socketadd *src)

➡️ same invalid type again

🔴 14. sizeof src_len
sizeof src_len

➡️ variable does not exist → compile error

🔴 15. Infinite loop with no handling (LOGIC ISSUE)
while (true)

➡️ you never:

break
handle multiple clients properly

(Not a compile error, but dangerous)

⚠️ Summary (Most Dangerous)

These will completely break your program:

❌ duplicate constructor
❌ socketadd type
❌ AF_INIT
❌ wrong bind() usage
❌ wrong accept() usage
❌ buff macro misuse
❌ clsoe typo
💡 Final Advice (important for ft_irc)

Before even testing:

Fix types
Fix function signatures
Fix pointer usage

👉 Right now your code won’t:

compile cleanly ❌
bind correctly ❌
accept connections ❌
