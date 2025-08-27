NAME = webserv

CC = g++
FLAGS = -g -Wall -Wextra -Werror -std=c++98 \
	-I./inc \
	-I./inc/config_classes \
	-I./inc/connection_handler \
	-I./inc/connection_handler/HTTP_actions \
	-I./inc/CGI

SRCS = \
	src/main.cpp \
	src/Path.cpp \
	src/Server/HTPPServerInit.cpp \
	src/Server/HTTPServer.cpp \
	src/Server/HTTPSserverCgi.cpp \
	src/connection_handler/ConnectionHandler.cpp \
	src/connection_handler/HTTPRequest.cpp \
	src/connection_handler/HTTPResponse.cpp \
	src/config/ConfigParser.cpp \
	src/config/MyConfig.cpp \
	src/config/ServerConfig.cpp \
	src/connection_handler/HTTP_actions/HTTPAction.cpp \
	src/connection_handler/HTTP_actions/HTTPActionDEL.cpp \
	src/connection_handler/HTTP_actions/HTTPActionGET.cpp \
	src/connection_handler/HTTP_actions/HTTPActionPOST.cpp \
	src/connection_handler/HTTP_actions/HTTPActionOPTIONS.cpp \
	src/connection_handler/HTTP_actions/MethodRegistry.cpp \
	src/CGI/Cgi.cpp \
	src/Timer.cpp \
	src/IntervalTimer.cpp

OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

obj:
	@mkdir -p obj/config
	@mkdir -p obj/connection_handler
	@mkdir -p obj/connection_handler/HTTP_actions
	@mkdir -p obj/Server
	@mkdir -p obj/CGI

obj/%.o: src/%.cpp | obj
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
