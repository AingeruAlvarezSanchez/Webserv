#SHELL = /bin/bash

NAME = webserv

CXX_FLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -Wshadow
SANITIZE = -g3 -fsanitize=address

INCLUDES = -I./includes/ -I./srcs/classes/

SRCS_DIR = srcs
SRCS = $(shell find $(SRCS_DIR) -type f -name "*.cpp")

OBJ_DIR = objs
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXX_FLAGS) $^ -o $@

sanitize: $(OBJS)
	$(CXX) $(CXX_FLAGS) $(SANITIZE) $^ -o $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp $(INCLUDE)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re