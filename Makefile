NAME        = ircserv
BONUS_NAME  = ircbot

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -I.
DEPFLAGS    = -MMD -MP

OBJ_DIR     = objs

# Mandatory part -------------------------------------------------------------

SRCS        = server/server.cpp \
              server/parsecmd.cpp \
              server/main.cpp \
              client/client.cpp \
              cmd/channel.cpp \
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
              bonus/main_test.cpp \
              server/parsecmd.cpp \
              server/server.cpp \
              client/client.cpp \
              cmd/channel.cpp \
              cmd/join.cpp \
              cmd/mode.cpp \
              cmd/privmsg.cpp \
              cmd/kick.cpp \
              cmd/invite.cpp \
              cmd/topic.cpp \
              cmd/PASS.cpp \
              cmd/NICK.cpp \
              cmd/USER.cpp

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
