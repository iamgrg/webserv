# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/19 12:24:42 by gregoire          #+#    #+#              #
#    Updated: 2023/11/12 14:10:44 by gregoire         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

# Compiler
CXX = c++

# Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Source files
SRC_FILES = \
	src/main.cpp \
	src/cgi/Cgi.cpp \
	src/config/Config.cpp \
	src/request/Request.cpp \
	src/location/Location.cpp \
	src/routes/Routes.cpp \
	src/response/Response.cpp \
	src/server/Server.cpp \
	src/utils/Utils.cpp 

# Object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Headers
INC_FLAGS = -I $(INC_DIR) -I $(INC_DIR)/external

# Rules
all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
