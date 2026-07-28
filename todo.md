user 
dont check  : with the realname
pass 
the client check just if it's connect after connect don't geet the password agin 
nickbame 
.. if the nickname exist this is the message 
nock moha
:ircserv 421  NOCK :Unknown command////error 
broadcast if nick change
in user ':' is not mandatory
---------------------------
➜  ft_irc git:(main) ✗ ./ircserv 8080 pass
create socket success
binding successe
PASSlistinig successenew client connected: fd 4
recv from fd 4: PASS pass
recv from fd 4: NICK mohos
recv from fd 4: USER 3 0 * 34
recv from fd 4: USER 1 2 3 :4
User registered: 1
Real name: 4
recv from fd 4: PONG ircserv
recv from fd 4: PONG ircserv
new client connected: fd 5
recv from fd 5: PASS pass
recv from fd 5: NICK sabir
recv from fd 5: USER sabir 0 * sabir
recv from fd 4: PONG ircserv
recv from fd 5: USER 1 2 3 :4
User registered: 1
Real name: 4
recv from fd 4: PONG ircserv
recv from fd 5: PONG ircserv
recv from fd 4: JOIN #g
channel: #g
recv from fd 5: JOIN #g
channel: #g
recv from fd 5: MODE #g
recv from fd 5: PRIVMSG #g hiuu9ef
recv from fd 4: PRIVMSG #g rthfgj
recv from fd 4: WHOIS sabir sabir
recv from fd 4: PONG ircserv
recv from fd 5: PONG ircserv
recv from fd 4: PRIVMSG sabir :DCC SEND Snitch_Animation.gif ::1 1096 625362
recv from fd 4: QUIT Leaving...
client disconnected: fd 4
recv from fd 5: PONG ircserv
recv from fd 5: QUIT Leaving...
client disconnected: fd 5
new client connected: fd 4
recv from fd 4: PASS pass
recv from fd 4: NICK mohos
recv from fd 4: USER 3 0 * 34
new client connected: fd 5
client disconnected: fd 5
new client connected: fd 5
recv from fd 5: PASS pass
recv from fd 5: NICK sabir
recv from fd 5: USER sabir 0 * sabir
recv from fd 4: USER 1 2 3 :4
User registered: 1
Real name: 4
recv from fd 4: JOIN #g
channel: #g
recv from fd 5: USER 1 2 3 :4
User registered: 1
Real name: 4
recv from fd 5: JOIN #g
channel: #g
recv from fd 5: MODE #g
ircserv(18419,0x10769edc0) malloc: Incorrect checksum for freed object 0x7fc776405a48: probably modified after being freed.
Corrupt value: 0x31
ircserv(18419,0x10769edc0) malloc: *** set a breakpoint in malloc_error_break to debug
[1]    18419 abort      ./ircserv 8080 pass