NAME = ircserv

COMPILE = g++ -std=c++98 -Wall -Wextra -Werror -g

SRCS = main.cpp Server.cpp Client.cpp Channel.cpp parsingServer.cpp utils.cpp modes.cpp \
processInvite.cpp processJoin.cpp processKick.cpp processList.cpp processMode.cpp \
processNames.cpp processPart.cpp processPing.cpp processPrivmsg.cpp processTopic.cpp \
processAway.cpp processNick.cpp processQuit.cpp

HEADERS = Server.hpp Client.hpp Channel.hpp

OBJPATH = .obj

OBJS = $(addprefix $(OBJPATH)/, $(SRCS:%.cpp=%.o))

$(OBJPATH)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJPATH)
	$(COMPILE) $(FLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	$(COMPILE) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(COMPILE) -c $< -o $@

clean:
	rm -rf $(OBJS) $(OBJPATH)

fclean: clean
	rm -f $(NAME)

re: fclean all
