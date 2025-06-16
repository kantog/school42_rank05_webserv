NAME = webserv

MODE ?= release

INCLUDES = -I./inc -I./inc/config_classes -I./inc/connection_handler -I./inc/connection_handler/HTTP_actions \
	-I./inc/CGI

FLAGS = -O0 -Wall -Wextra -Werror -std=c++98 -g3 -Wshadow $(INCLUDES)

ifeq ($(MODE),debug)
	CC = g++
	FLAGS += -DDEBUG 
	DEBUG_FLAGS =
	JSON_FLAG =
else
	CC = clang++
	FLAGS += --target=x86_64-pc-linux-gnu
	DEBUG_FLAGS =
	JSON_FLAG = -MJ $@.json
endif

# Lijst van alle bronbestanden mét hun submappen
SRCS = \
	src/main.cpp \
	src/Server/HTPPServerInit.cpp \
	src/Server/HTTPServer.cpp \
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
	src/connection_handler/HTTP_actions/MethodRegistry.cpp \
	src/CGI/Cgi.cpp

# Zet object-bestanden in obj/ met dezelfde mappenstructuur
OBJECTS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) -o $@ $(OBJECTS)
ifeq ($(MODE),release)
	@echo "\033[33mMaking compile_commands.json...\033[0m"
	@find . -type f -name "compile_commands.json" -delete
	@find ./obj/ -type f -name "*.json" | xargs sed -e '1s/^/[\n/' >> compile_commands.json
	@find ./obj/ -type f -name "*.json" -delete
endif
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"
	@echo "\033[32m+++++    All set!    +++++\033[0m"
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"

# Maak mappen aan voor object bestanden (recursive)
obj:
	@mkdir -p obj/config
	@mkdir -p obj/connection_handler
	@mkdir -p obj/connection_handler/HTTP_actions
	@mkdir -p obj/Server
	@mkdir -p obj/CGI


# Compileer per source file met correcte pad
obj/%.o: src/%.cpp | obj
	$(CC) $(FLAGS) $(JSON_FLAG) -c -o $@ $<

clean:
	@rm -rf obj
	@echo "\033[33mAll object files, dependency files, archive files and test files removed.\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[33mBinary removed.\033[0m"

aclean: fclean
	@rm -rf compile_commands.json
	@rm -rf Session.vim
	@echo "\033[32m+++++++++++++++++++++++++++++++++++\033[0m"
	@echo "\033[32m+++++    All set for eval!    +++++\033[0m"
	@echo "\033[32m+++++++++++++++++++++++++++++++++++\033[0m"

re: fclean all

.PHONY: all clean fclean re aclean obj
