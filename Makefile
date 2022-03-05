NAME		=	chip8

SRC			=	src/main.c						\
				src/disas.c						\
				src/op_codes.c					\
				src/chip8_engine.c				\
				src/utils.c						\
				src/instructions_executors.c	\
				src/display_buffer.c			\
				src/clock.c						\
				src/display.c

CC			=	gcc

OBJ			=	$(SRC:.c=.o)

CFLAGS		=	-Wall -Wextra

CPPFLAGS	=	-I./inc

LIBFLAGS	=	-lSDL2

RM			=	rm -f

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LIBFLAGS)

build: all

clean:
	@$(RM) $(OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

debug: CPPFLAGS += -g3
debug: re

.PHONY: all clean fclean re build debug