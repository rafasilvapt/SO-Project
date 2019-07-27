run:
	@gcc notebook.c -o notebook
	@./notebook notebook.nb
remove:
	@rm -r outputs error.txt temp.txt notebook
