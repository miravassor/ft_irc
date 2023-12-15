NAME = servirc

COMPILE = c++ -std=c++98 -Wall -Wextra -Werror

SRCS = main.cpp Server.cpp Client.cpp Channel.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): Server.hpp Client.hpp Channel.hpp $(OBJS)
	$(COMPILE) $(OBJS) -o $(NAME)

%.o: %.cpp %.hpp
	$(COMPILE) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all