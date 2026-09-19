NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS_DIR = srcs
OBJS_DIR = objs
INC_DIR = includes

SRCS = main.c \
       args_check.c \
       args_parse.c \
       error_utils.c \
       time_utils.c \
       sim_stop.c \
       logger.c \
       heap.c \
       heap_sift.c \
       heap_remove.c \
       scheduler.c \
       dongle_acquire.c \
       dongle_release.c \
       coder_dongles.c \
       coder_cycle.c \
       coder_thread.c \
       monitor_thread.c \
       init_dongles.c \
       init_coders.c \
       init_simulation.c \
       free_simulation.c

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(INC_DIR)/codexion.h | $(OBJS_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
