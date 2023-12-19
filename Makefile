NAME = servirc

COMPILE = g++ -std=c++98 -Wall -Wextra -Werror -g

SRCS = main.cpp Server.cpp parsingServer.cpp Client.cpp Channel.cpp processCmd.cpp modes.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): Server.hpp Client.hpp Channel.hpp $(OBJS)
	$(COMPILE) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(COMPILE) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
