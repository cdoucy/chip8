NAME	=	chip8

SRC		=	main.c disas.c

CC		=	gcc

OBJ		=	$(SRC:.c=.o)

CFLAGS	=	-Wall -Wextra

RM		=	rm -f

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