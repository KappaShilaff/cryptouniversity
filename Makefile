NAME = crypto

all:  $(NAME)

$(NAME):
	@gcc -o $(NAME) crypto.c -lgcrypt
	@echo "$(NAME) was created."

fclean:  
	@rm -rf $(NAME)	

re: fclean all

