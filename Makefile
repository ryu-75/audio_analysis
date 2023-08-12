# AJOUTER => Vérifier si libsndfile1-dev est installer en vérifiant avec "libsndfile1-dev --version" ; SI OUI installe la librairie; SI NON rien faire

# .................. COLORS ...................................

_BOLD      =\e[1m

# RESET list
_R          =\e[0m

# Colors
_RED      =\e[91m
_GREEN    =\e[92m
_YELLOW   =\e[93m
_BLUE     =\e[94m
_MAGENTA  =\e[35m
_CYAN     =\e[96m
_WHITE    =\e[97m

NAME		= AudioAnalysis
EXEC		= $(NAME)

# ................... CONTENT .................................

SRC_FILES		= 	main.cpp		\
					AudioAn.cpp	\

# ................... OBJECT ..................................

OBJ_DIR			= obj/
OBJS			= $(addprefix $(OBJ_DIR), $(SRC_FILES:%.cpp=%.o))
$(OBJ_DIR)%.o	: 	%.cpp AudioAn.hpp
					@mkdir -p $(OBJ_DIR)
					@$(CPP) $(CPP_FLAG) $(FLAGS) -c $< -o $@

# ................... COMPILER ................................

CPP				= c++
CPP_FLAG		= -std=c++17
FLAGS			= -Wall -Wextra -Werror -g
$(EXEC)			: $(OBJS)
					@$(CPP) $(CPP_FLAG) $(FLAGS) $(OBJS) -o $(EXEC) -lsndfile -lfftw3
					@printf "$(_RED)$(_BOLD)Compiled : $(_WHITE)$(EXEC)$(_R)\n\n"
					@printf "$(CPP) $(_BOLD)$(_GREEN)$(CPP_FLAG) $(_BLUE)$(FLAGS) $(_WHITE)main.cpp$(_R)\n"
					@printf "$(CPP) $(_BOLD)$(_GREEN)$(CPP_FLAG) $(_BLUE)$(FLAGS) $(_WHITE)AudioAn.cpp$(_R)\n\n"
					@printf "$(_BOLD)$(_GREEN)................. Compilation succed .................$(_R)\n\n"

all				: $(EXEC)

clean			:
					@rm -rf $(OBJ_DIR)
					@printf "\n$(_MAGENTA)Removed : $(_WHITE)./$(OBJ_DIR)\n"

fclean			: clean
					@rm -f $(EXEC)
					@printf "$(_MAGENTA)Removed : $(_WHITE)./$(EXEC)\n"
					@printf "$(_GREEN)All is clean now !\n$(_R)"

re				: fclean
					@make all

.PHONY			= all clean fclean re
