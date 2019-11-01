/*
 ============================================================================
 Name        : expr_ast.c
 Author      : catfee
 Version     : demo
 Copyright   : n
 Description : compile tech
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//enum {
//	ASSIGN = 258, STR, CMP, SCAN, PRINT,
//	RETURN, WHILE, IF, ELSE, VOID, INT,
//	ID, STRING, NUMBER, ERROR
//};
/*add for AST begin*/

typedef enum _node_type_type node_type_type;
typedef struct _ast_node ast_node;
typedef struct _child_node_ptr child_node_ptr;
typedef struct _ast ast;

//用来定义结点类型的枚举类型变量
enum _node_type_type{
	ASSIGN = 258, STR, CMP, SCAN, PRINT,
	RETURN, WHILE, IF, ELSE, VOID, INT,
	ID, STRING, NUMBER, ERROR,

	PROGRAM, EXTERNAL_DECLARATION, DECL_OR_STMT, DECLARATOR_LIST,
	INTSTR_LIST, INITIALIZER, DECLARATOR, PARAMETER_LIST,
	PARAMETER, TYPE, STATEMENT,
	STATEMENT_LIST, EXPRESSION_STATEMENT, EXPR,
	CMP_EXPR, ADD_EXPR, MUL_EXPR,
	PRIMARY_EXPR, EXPR_LIST, ID_LIST
};

//结点结构类型定义
struct _ast_node {
	node_type_type node_type;
	child_node_ptr *child_list;//用链表储存孩子节点的指针
};

//孩子结点链表的结点
struct _child_node_ptr {
	ast_node child_node;
	struct _child_node_ptr *next;
};

//抽象语法树的结构类型
struct _ast {
	ast_node *root;
};


/**add for AST end*/




extern int yylex();
extern int yylval;
extern char* yytext;
extern FILE* yyin;

extern int cur_line_num;



int find_error();

int program();
int external_declaration();
int decl_or_stmt();
int declarator_list();
int intstr_list();
int initializer();
int declarator();
int parameter_list();
int parameter();
int type();
int statement();
int statement_list();
int expression_statement();
int expr();
int cmp_expr();
int add_expr();
int mul_expr();
int primary_expr();
int expr_list();
int id_list();

int tok;


void advance()
{
	tok = yylex();
	printf("tok: %s\n", yytext);
}



/* My function********************/

int find_error() {
	printf("here is an error at line: %d\n", cur_line_num);
	advance();
	return 1;
}

//program
//	: external_declaration
//	| program external_declaration
//	;
int program() {
	external_declaration();
	while ((EOF != tok) && (0 != tok)) {
		external_declaration();
	}
	return 1;
}

//external_declaration
//	: type declarator decl_or_stmt
//	;
int external_declaration() {
	type();
	declarator();
	decl_or_stmt();

	return 1;
}

//decl_or_stmt
//	: '{' statement_list '}'
//	| '{' '}'
//	| ',' declarator_list ';'
//	| ';'
//	;
int decl_or_stmt() {
	if ('{' == tok) {
		advance();
		if ('}' != tok) {
			statement_list();
			if ('}' != tok) {
				find_error();
			}
			else {
				advance();
			}
		}
		else {
			advance();
		}
	}
	else if (',' == tok) {
		advance();
		declarator_list();
		if (';' != tok) {
			find_error();
		}
		else {
			advance();
		}
	}
	else if (';' == tok) {
		advance();
	}
	else {
		find_error();
	}

	return 1;
}

//declarator_list
//	: declarator
//	| declarator_list ',' declarator
//	;
int declarator_list() {
	declarator();
	while (',' == tok) {
		advance();
		declarator();
	}

	return 1;
}

//intstr_list
//	: initializer
//	| intstr_list ',' initializer
//	;
int intstr_list() {
	initializer();
	while (',' == tok) {
		advance();
		initializer();
	}

	return 1;
}

//initializer
//	: NUMBER
//	| STRING
//	;
int initializer() {
	if ((NUMBER != tok) && (STRING != tok)) {
		find_error();
	}
	else {
		advance();
	}

	return 1;
}

//declarator
//	: ID
//	| ID '=' expr
//	| ID '(' parameter_list ')'
//	| ID '(' ')'
//	| ID '[' expr ']'
//	| ID '[' ']'
//	| ID '[' expr ']' '=' '{' intstr_list '}'
//	| ID '[' ']' '=' '{' intstr_list '}'
//	;
int declarator() {
	if (ID != tok) {
		find_error();
	}
	else {
		advance();
		if ('=' == tok) {
			advance();
			expr();
		}
		else if ('(' == tok) {
			advance();
			if (')' != tok) {
				parameter_list();
			}
			advance();
		}
		else if ('[' == tok) {
			advance();
			if (']' == tok) {
				advance();
				if ('=' == tok) {
					advance();
					if ('{' != tok) {
						find_error();
					}
					else {
						intstr_list();
						if ('}' != tok) {
							find_error();
						}
						else {
							advance();
						}
					}
				}
			}
			else {
				expr();
				if (']' != tok) {
					find_error();
				}
				else {
					if ('=' == tok) {
						if ('{' != tok) {
							find_error();
						}
						else {
							intstr_list();
							if ('}' != tok) {
								find_error();
							}
							else {
								advance();
							}
						}
					}
				}
			}
		}
	}

	return 1;
}

//parameter_list
//	: parameter
//	| parameter_list ',' parameter
//	;
int parameter_list() {
	parameter();
	while (',' == tok) {
		advance();
		parameter();
	}
	return 1;
}

//parameter
//	: type ID
//	;
int parameter() {
	type();
	if (ID != tok) {
		find_error();
	}
	else {
		advance();
	}
	return 1;
}

//type
//	: INT
//	| STR
//	| VOID
//	;
int type() {
	if ((INT != tok) && (STR != tok) && (VOID != tok)) {
		find_error();
	}
	else {
		advance();
	}
	return 1;
}

//statement
//	: type declarator_list ';'
//	| '{' statement_list '}'
//	| expr_statement
//	| IF '(' expr ')' statement
//	| IF '(' expr ')' statement ELSE statement
//	| WHILE '(' expr ')' statement
//	| RETURN ';'
//	| RETURN expr ';'
//	| PRINT ';
//	| PRINT expr_list ';'
//	| SCAN id_list ';'
//	;
int statement() {
	if ('{' == tok) {
		advance();
		statement_list();
		if ('}' != tok) {
			find_error();
		}
		else {
			advance();
		}
	}
	else if (IF == tok) {
		advance();
		if ('(' != tok) {
			find_error();
		}
		else {
			advance();
			expr();
			if (')' != tok) {
				find_error();
			}
			else {
				advance();
				statement();
				if (ELSE == tok) {
					advance();
					statement();
				}
			}
		}
	}
	else if (WHILE == tok) {
		advance();
		if ('(' != tok) {
			find_error();
		}
		else {
			advance();
			expr();
			if (')' != tok) {
				find_error();
			}
			else {
				advance();
				statement();
			}
		}
	}
	else if (RETURN == tok) {
		advance();
		if (';' == tok) {
			advance();
		}
		else {
			expr();
			if (';' != tok) {
				find_error();
			}
			else {
				advance();
			}
		}
	}
	else if (PRINT == tok) {
		advance();
		if (';' == tok) {
			advance();
		}
		else {
			expr_list();
			if (';' != tok) {
				find_error();
			}
			else {
				advance();
			}
		}
	}
	else if (SCAN == tok) {
		id_list();
		if (';' != tok) {
			find_error();
		}
		else {
			advance();
		}
	}
	else if ((INT == tok) || (STR == tok) || (VOID == tok)) {
		type();
		declarator_list();
		if (';' != tok) {
			find_error();
		}
		else {
			advance();
		}
	}
	else {
		expression_statement();
	}
	return 1;
}

//statement_list
//	: statement
//	| statement_list statement
//	;
int statement_list() {
	statement();
	while ((EOF != tok) && ('}' != tok)) {
		statement();
	}
	/*if ('}' == tok) {
		advance();
	}*/
	return 1;
}

//expression_statement
//	: ';'
//	| expr ';'
//	;
int expression_statement() {
	if (';' == tok) {
		advance();
	}
	else {
		expr();
		if (';' != tok) {
			find_error();
		}
		else {
			advance();
		}
	}
	return 1;
}

//expr
//	: cmp_expr
//	;
int expr() {
	cmp_expr();
	return 1;
}

//cmp_expr
//	: add_expr
//	| cmp_expr CMP add_expr
//	;
int cmp_expr() {
	add_expr();
	while (CMP == tok) {
		advance();
		add_expr();
	}
	return 1;
}

//add_expr
//	: mul_expr
//	| add_expr '+' mul_expr
//	| add_expr '-' mul_expr
//	;
int add_expr() {
	int oper;
	mul_expr();
	while (('+' == tok) || ('-' == tok)) {
		advance();
		mul_expr();
	}
	return 1;
}

//mul_expr
//	: primary_expr
//	| mul_expr '*' primary_expr
//	| mul_expr '/' primary_expr
//	| mul_expr '%' primary_expr
//	| '-' primary_expr
//	;
int mul_expr() {
	int oper = 0;//运算符
	if ('-' == tok) {
		advance();
		primary_expr();
	}
	else {
		primary_expr();
		while (('*' == tok) || ('/' == tok) || ('%' == tok)) {
			oper = tok;
			advance();
			primary_expr();
		}
	}
	return 1;
}

//primary_expr
//	: ID '(' expr_list ')'
//	| ID '(' ')'
//	| '(' expr ')'
//	| ID
//	| NUMBER
//	| STRING
//	| ID ASSIGN expr
//	| ID '=' expr
//	| ID '[' expr ']'
//	| ID '[' expr ']' '=' expr
//	;
int primary_expr() {
	switch (tok) {
	case ID:
		advance();
		if ('(' == tok) {
			advance();
			if (')' != tok)
				expr_list();
			if (')' == tok)
				advance();
		}
		else if ('=' == tok) {
			advance();
			expr();
		}
		else if ('[' == tok) {
			advance();
			expr();
			if (']' != tok) {
				find_error();
				//return 0;
			}
			else {
				advance();
				if ('=' != tok) {
					find_error();
				}
				else {
					advance();
					expr();
				}
			}
		}
		else if (ASSIGN == tok) {
			advance();
			expr();
		}
		break;
	case '(':
		advance();
		expr();
		if (')' != tok) {
			find_error();
		}
		else {
			advance();
		}
		break;
	case NUMBER:
		advance();
		break;
	case STRING:
		advance();
		break;
	default:
		find_error();
	}

	return 1;
}

//expr_list
//	: expr
//	| expr_list ',' expr
//	;
int expr_list() {
	expr();
	while (tok == ',') {
		advance();
		expr();
	}

	return 1;
}

//id_list
//	: ID
//	| id_list ',' ID
//	;
int id_list() {
	int l = ID;//L
	advance();
	while (tok == ',') {
		advance();
		if (tok != ID) {
			find_error();
		}
		else {
			advance();
		}
	}
	return 1;//one
}


//
//typedef struct _ast ast;
//typedef struct _ast *past;
//struct _ast {
//	int ivalue;
//	char* nodeType;
//	past left;
//	past right;
//};
//
//past newAstNode()
//{
//	past node = malloc(sizeof(ast));
//	if (node == NULL)
//	{
//		printf("run out of memory.\n");
//		exit(0);
//	}
//	memset(node, 0, sizeof(ast));
//	return node;
//}
//
//past newNum(int value)
//{
//	past var = newAstNode();
//	var->nodeType = "intValue";
//	var->ivalue = value;
//	return var;
//}
//past newExpr(int oper, past left, past right)
//{
//	past var = newAstNode();
//	var->nodeType = "expr";
//	var->ivalue = oper;
//	var->left = left;
//	var->right = right;
//	return var;
//}
//
//past astTerm()
//{
//	if (tok == NUMBER)
//	{
//		past n = newNum(yylval);
//		advance();
//		return n;
//	}
//	else if (tok == '-')
//	{
//		advance();
//		past k = astTerm();
//		past n = newExpr('-', NULL, k);
//		return n;
//	}
//	else if (tok == 'q')
//		exit(0);
//	return NULL;
//}
//
//
//past astFactor()
//{
//	past l = astTerm();
//	while (tok == '*' || tok == '/')
//	{
//		int oper = tok;
//		advance();
//		past r = astTerm();
//		l = newExpr(oper, l, r);
//	}
//	return l;
//}
//
//past astExpr()
//{
//	past l = astFactor();
//	while (tok == '+' || tok == '-')
//	{
//		int oper = tok;
//		advance();
//		past r = astFactor();
//		l = newExpr(oper, l, r);
//	}
//	return l;
//}
//
//void showAst(past node, int nest)
//{
//	if (node == NULL)
//		return;
//
//	int i = 0;
//	for (i = 0; i < nest; i++)
//		printf("  ");
//	if (strcmp(node->nodeType, "intValue") == 0)
//		printf("%s %d\n", node->nodeType, node->ivalue);
//	else if (strcmp(node->nodeType, "expr") == 0)
//		printf("%s '%c'\n", node->nodeType, (char)node->ivalue);
//	showAst(node->left, nest + 1);
//	showAst(node->right, nest + 1);
//
//}

int main(int argc, char **argv)
{
	//	if(argc != 2 )
	//	{
	//		printf("input file is needed.\n");
	//		return 0;
	//	}
	//	FILE* f = fopen(argv[1]);
	setbuf(stdout, NULL);
	yyin = fopen("test.c", "r");
	advance();
	program();
	printf("anal finished!\n");
	//past rr = astExpr();
	//showAst(rr, 0);

	return 0;
}
