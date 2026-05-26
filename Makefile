NAME = codexion

CFLAGS = -Wall -Wextra -Werror -pthread -I/coders
CC = cc
RM = rm -f

SRC_DIR = ./coders
OBJ_DIR = temp

SRC = \
		$(SRC_DIR)/main.c \
		$(SRC_DIR)/codexion_input_validation.c \
		$(SRC_DIR)/codexion_errors.c \
		$(SRC_DIR)/codexion_create_simulation.c \
		$(SRC_DIR)/codexion_time.c \
		$(SRC_DIR)/codexion_coder_routine.c \
		$(SRC_DIR)/codexion_coder_routine_queue.c \
		$(SRC_DIR)/codexion_coder_routine_grab_dongles.c \
		$(SRC_DIR)/codexion_coder_routine_release_dongle.c \
		$(SRC_DIR)/codexion_monitor_routine.c \
		$(SRC_DIR)/codexion_run_simulation.c \
		$(SRC_DIR)/codexion_cleanup.c \
		$(SRC_DIR)/codexion_test_prints.c

# --- colours ---
RESET := \033[0m
BOLD  := \033[1m
RED   := \033[31m
GRN   := \033[32m
YEL   := \033[33m
BLU   := \033[34m

OBJ = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.c=.o)))

$(NAME): $(OBJ)
		@printf "Compiling the $(BOLD)'codexion'$(RESET) program"
		@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) && \
		printf "%-10s[$(GRN)OK$(RESET)]\n" || \
		(printf "[$(RED)FAIL$(RESET)]\n"; exit 1)
		@printf "$(YEL)Run the program as follows:\n$(RESET)"
		@printf "./codexion <number_of_coders> <time_to_burnout> <time_to_compile>\n"
		@printf "           <time_to_debug> <time_to_refactor> <compiles_required>\n"
		@printf "           <dongle_cooldown> <scheduler>\n"
		@printf "$(YEL)(time is in ms, scheduler = fifo/edf)\n$(RESET)"
# 		@printf "$(YEL)(time is in ms, scheduler = fifo/edf (first in - first out / earliest deadline first))\n$(RESET)"

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@printf "CC %-35s" "$<"
	@$(CC) $(CFLAGS) -c $< -o $@ && \
		printf "[$(GRN)OK$(RESET)]\n" || \
		(printf "[$(RED)FAIL$(RESET)]\n"; exit 1)

clean:
	@$(RM) -r $(OBJ_DIR)
	@printf "$(BLU)Codexion objects have been deleted.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(BLU)Codexion program has been deleted.$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re