#SHELL = /bin/bash

NAME = webserv

CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -Wshadow -g3 -fsanitize=address
INCLUDES = -I./includes/

OBJDIR := objs/
SRCS = $(shell find ./srcs -type f -name *.cpp)
HEADERS = $(shell find ./srcs -type f -name *.hpp)
OBJS = $(addprefix $(OBJDIR), $(notdir $(SRCS:.cpp=.o)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJDIR)%.o:srcs/*/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)%.o:srcs/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	$(RM) -r $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re