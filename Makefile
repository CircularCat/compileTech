# part of the makefile



all:  lex.yy.c expr_ast.c
	gcc -o parser lex.yy.c expr_ast.c

clean:
	rm parser
