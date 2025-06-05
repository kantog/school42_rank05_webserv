NAME = webserv

# Default mode: release (clang++)
MODE ?= release

ifeq ($(MODE),debug)
	CC = g++
	FLAGS = -DDEBUG -O0 -Wall -Wextra -Werror -std=c++98 -g3 -Wshadow
	DEBUG_FLAGS =
	JSON_FLAG = # g++ doesn't support -MJ
else
	CC = clang++
	FLAGS = -DDEBUG -O0 -Wall -Wextra -Werror -std=c++98 -g3 -Wshadow --target=x86_64-pc-linux-gnu
	DEBUG_FLAGS =
	JSON_FLAG = -MJ $@.json
endif

SRCS = main.cpp HTTPServer.cpp ConnectionHandler.cpp \
		ConfigParser.cpp MyConfig.cpp \
		HTTPRequest.cpp HTTPResponse.cpp

SRC_DIRS = src/

vpath %.cpp $(SRC_DIRS)

OBJECTS = $(patsubst %.cpp,obj/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJECTS)
ifeq ($(MODE),release)
	@echo "\033[33mMaking compile_commands.json...\033[0m"
	@find . -type f -name "compile_commands.json" -delete
	@find ./obj/ -type f -name "*.json" | xargs sed -e '1s/^/[\n/' >> compile_commands.json
	@find ./obj/ -type f -name "*.json" -delete
endif
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"
	@echo "\033[32m+++++    All set!    +++++\033[0m"
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"

obj:
	@mkdir -p obj

obj/%.o: %.cpp | obj
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

.PHONY: all clean fclean re aclean
