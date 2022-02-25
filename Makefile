NAME		=	chip8

SRC			=	src/main.c		\
				src/disas.c		\
				src/op_codes.c

CC			=	gcc

OBJ			=	$(SRC:.c=.o)

CFLAGS		=	-Wall -Wextra

CPPFLAGS	=	-I./inc

RM			=	rm -f

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ)

build: all

clean:
	@$(RM) $(OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re build