# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/08 11:15:03 by bclaeys           #+#    #+#              #
#    Updated: 2025/06/04 16:54:47 by kvanden-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = clang++ #reason: c++ (gcc) is not up to date on many c19 computers
FLAGS = -DDEBUG -O0 -Wall -Wextra -Werror -std=c++98 -g3 -Wshadow --target=x86_64-pc-linux-gnu
DEBUG_FLAGS= #-fsanitize=address -fsanitize=undefined -fsanitize=bounds -fsanitize=null

SRCS = main.cpp HTTPServer.cpp ConnectionHandler.cpp \
		ConfigParser.cpp MyConfig.cpp \
		HTTPRequest.cpp HTTPResponse.cpp


SRC_DIRS = src/

vpath %.cpp $(SRC_DIRS)  

OBJECTS = $(patsubst %.cpp,obj/%.o,$(SRCS))

all: $(NAME)  

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJECTS)
	@echo "\033[33mMaking compile_commands.json...\033[0m"
	@find . -type f -name "compile_commands.json" -delete
	@find ./obj/ -type f -name "*.json" | xargs sed -e '1s/^/[\n/' >> compile_commands.json
	@find ./obj/ -type f -name "*.json" -delete
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"
	@echo "\033[32m+++++    All set!    +++++\033[0m"
	@echo "\033[32m++++++++++++++++++++++++++\033[0m"

obj:
	@mkdir obj

obj/%.o: %.cpp | obj
	$(CC) $(FLAGS) -MJ $@.json -c -o $@ $<

clean:
	@rm -rf obj
	@if [ -d "obj" ]; then rmdir obj/; \
	fi
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

