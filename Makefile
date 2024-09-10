CC = c++

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -g -Iincludes

NAME = webserv 

SRCSDIR = srcs
OBJSDIR = $(SRCSDIR)/objs

SRCS = main ServerManager Server global StreamReader \
	enum/MimeType enum/HttpMethod enum/HttpStatusCode \
	request/HTTPPayload request/Request request/Response request/AutoIndexGenerator request/CGIHandler request/Client request/CookiesManager \
	config/AConfiguration config/ConfigParser config/Route config/ServerConfig config/JSON config/Pair

OBJS = $(addprefix $(OBJSDIR)/, $(SRCS:=.o))

RED = \033[0;31m
GREEN = \033[0;32m
BLUE = \033[94m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME)
	@echo "$(BLUE)created: $(RESET)$(NAME)"

$(OBJS): $(OBJSDIR)/%.o: $(SRCSDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) -c $< -o $@
	@echo "$(GREEN)compiled: $(RESET)$<"

clean:
	@rm -rf $(OBJSDIR)
	@echo "$(RED)deleted: $(RESET)$(OBJSDIR)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)deleted: $(RESET)$(NAME)"

re: fclean all

.PHONY: all clean fclean re
