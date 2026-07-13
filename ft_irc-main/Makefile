# |___bonus
# |___|___bot.cpp
# |___|___bot.hpp
# |___|___main_test.cpp
# |___client
# |___|___client.cpp
# |___|___client.hpp
# |___cmd
# |___|___cmd.hpp
# |___|___join.cpp
# |___|___mode.cpp
# |___server
# |___|___main.cpp
# |___|___server.cpp
# |___|___server.hpp
# |___Makefile
# |___main.cpp


NAME        = ircserv
BONUS_NAME  = ircbot

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -I.
DEPFLAGS    = -MMD -MP

OBJ_DIR     = objs

# Mandatory part -------------------------------------------------------------
# NOTE: no file in the repo currently defines int main() for the server
# itself (server/main.cpp is missing). Add one that calls start_server(),
# otherwise linking $(NAME) will fail with "undefined reference to main".
SRCS        = server/server.cpp \
              server/main.cpp \
              client/client.cpp \
              cmd/join.cpp \
              cmd/mode.cpp \
              cmd/privmsg.cpp \
              cmd/kick.cpp \
              cmd/invite.cpp \
              cmd/topic.cpp \
              cmd/PASS.cpp \
              cmd/NICK.cpp \
              cmd/USER.cpp

# Bonus part (built separately, not linked into ircserv) ---------------------
BONUS_SRCS  = bonus/bot.cpp \
              bonus/main_test.cpp

OBJS        = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
BONUS_OBJS  = $(addprefix $(OBJ_DIR)/, $(BONUS_SRCS:.cpp=.o))
DEPS        = $(OBJS:.o=.d) $(BONUS_OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "$(NAME) built"

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "$(BONUS_NAME) built"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all bonus clean fclean re