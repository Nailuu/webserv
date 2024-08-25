CC = c++

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -g

NAME = webserv 

SRCSDIR = srcs
OBJSDIR = $(SRCSDIR)/objs

SRCS = main ServerManager Server global \
	request/HTTPPayload request/Request request/Response \
	config/ConfigParser config/Route config/ServerConfig config/JSON config/Pair

OBJS = $(addprefix $(OBJSDIR)/, $(SRCS:=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

$(OBJS): $(OBJSDIR)/%.o: $(SRCSDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJSDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
