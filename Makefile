NAME = zim

CC = cc
CFLAGS = -Wall -Wextra -Werror -g3

# Find all .c files recursively in src directory
SRCS = $(shell find src -type f -name "*.c")
OBJS = $(SRCS:.c=.o)




all: subsystems $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS)  -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re subsystems