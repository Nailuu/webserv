CC = c++

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -g

NAME = webserv 

SRCSDIR = srcs
OBJSDIR = $(SRCSDIR)/objs

SRCS = main ServerManager Server global Client \
	enum/MimeType enum/HttpMethod enum/HttpStatusCode enum/AcceptedCGI \
	request/HTTPPayload request/Request request/Response request/AutoIndexGenerator \
	config/ConfigParser config/Route config/ServerConfig config/JSON config/Pair config/CGI

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
