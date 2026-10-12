NAME = libftpp.a

CPP = c++
STD = -std=c++23

FLAGS = -Wall -Wextra -Werror ${STD}

SRC_FILES =	"data-structures/data-buffer/data_buffer.cpp"

OBJ_FILES = $(pathsub %, ${OBJ_DIR}/%, $(SRC_FILES:.c=.o))

HEADER = "include/libftpp.hpp"
HEADER_FLAG = "-Iinclude"
OBJ_DIR = obj

RM = rm -rf
# -D_GLIBCXX_ASSERTIONS
debug:
	@c++ -Wall -Werror -Wextra -g -fsanitize=address  --std=c++20 main.cpp && ASAN_OPTIONS=detect_stack_use_after_return=1 ./a.out
run:
	@c++ -Wall -Werror -Wextra --std=c++20 main.cpp && ./a.out
sanitize:
	@c++ -Wall -Werror -Wextra -g -fsanitize=address --std=c++20 main.cpp && ./a.out



all: $(NAME)


$(NAME) : $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.c $(HEADER)
	mkdir -p $(dir $@)
	$(CPP) $(FLAGS) $(HEADER_FLAG) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

# .Phony: 
# .SILENT
.DEFAULT_GOAL: all