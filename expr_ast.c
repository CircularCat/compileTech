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
	child_node_ptr *child_list;//用 带头结点的 链表储存孩子节点的指针
};

//孩子结点链表的结点
struct _child_node_ptr {
	ast_node *child_node;
	child_node_ptr *next;
};

//抽象语法树的结构类型
struct _ast {
	ast_node *root;
};

const char* type_form[] = {
	"ASSIGN", "STR", "CMP", "SCAN", "PRINT",
	"RETURN", "WHILE", "IF", "ELSE", "VOID", "INT",
	"ID", "STRING", "NUMBER", "ERROR",

	"PROGRAM", "EXTERNAL_DECLARATION", "DECL_OR_STMT", "DECLARATOR_LIST",
	"INTSTR_LIST", "INITIALIZER", "DECLARATOR", "PARAMETER_LIST",
	"PARAMETER", "TYPE", "STATEMENT",
	"STATEMENT_LIST", "EXPRESSION_STATEMENT", "EXPR",
	"CMP_EXPR", "ADD_EXPR", "MUL_EXPR",
	"PRIMARY_EXPR", "EXPR_LIST", "ID_LIST"
};


/**add for AST end*/



//flex生成的自动机的部分函数以及变量
extern int yylex();
extern int yylval;
extern char* yytext;
extern FILE* yyin;

extern int cur_line_num;


//函数声明

ast *create_ast(void);
ast_node *create_ast_node(node_type_type node_type);
ast_node * add_child_node_ptr(ast_node * father, node_type_type child_node_type);
int show_ast(ast *root);
int show_node(ast_node *node, int space_num);
//child_node_ptr *add_child_node_ptr(ast_node *father, ast_node *child);

int find_error();

ast_node *program();
ast_node *external_declaration(ast_node *root);
ast_node *decl_or_stmt(ast_node *root);
ast_node *declarator_list(ast_node *root);
ast_node *intstr_list(ast_node *root);
ast_node *initializer(ast_node *root);
ast_node *declarator(ast_node *root);
ast_node *parameter_list(ast_node *root);
ast_node *parameter(ast_node *root);
ast_node *type(ast_node *root);
ast_node *statement(ast_node *root);
ast_node *statement_list(ast_node *root);
ast_node *expression_statement(ast_node *root);
ast_node *expr(ast_node *root);
ast_node *cmp_expr(ast_node *root);
ast_node *add_expr(ast_node *root);
ast_node *mul_expr(ast_node *root);
ast_node *primary_expr(ast_node *root);
ast_node *expr_list(ast_node *root);
ast_node *id_list(ast_node *root);

int tok;

/*声明创建语法树所需的函数 begin*/

//创建新语法树，返回其指针
ast *create_ast(void) {
	ast *new_ast = (ast*)malloc(sizeof(ast));
	memset(new_ast, 0, sizeof(ast));
	return new_ast;
}

//创建结点，以及含头结点的孩子链表，并返回其指针
ast_node *create_ast_node(node_type_type node_type) {
	ast_node *new_ast_node = (ast_node*)malloc(sizeof(ast_node));
	memset(new_ast_node, 0, sizeof(ast_node));

	new_ast_node->node_type = node_type;//为结点赋值，为当前的语法符号

	child_node_ptr *new_child_node_ptr = (child_node_ptr*)malloc(sizeof(child_node_ptr));//创建孩子链表的头结点
	memset(new_child_node_ptr, 0, sizeof(child_node_ptr));
	new_ast_node->child_list = new_child_node_ptr;

	char* licence = "catfee";

	return new_ast_node;
}

//增加孩子链表结点,结点值为第二个参数,返回该结点指针
/*
child_node_ptr *add_child_node_ptr(ast_node *father, node_type_type child_node_type) {
	child_node_ptr *new_child_node_ptr = (child_node_ptr*)malloc(sizeof(child_node_ptr));//创建孩子链表结点
	memset(new_child_node_ptr, 0, sizeof(child_node_ptr));

	new_child_node_ptr->child_node = create_ast_node(child_node_type);

	//插入结点到尾部，由于使用了头结点，尾插法较容易实现
	child_node_ptr *p = father->child_list;
	while (NULL != p->next) {
		p = p->next;
	}
	p->next = new_child_node_ptr;

	return new_child_node_ptr;
}
*/
//新版创建结点
ast_node *add_child_node_ptr(ast_node *father, node_type_type child_node_type) {
	child_node_ptr *new_child_node_ptr = (child_node_ptr*)malloc(sizeof(child_node_ptr));//创建孩子链表结点
	memset(new_child_node_ptr, 0, sizeof(child_node_ptr));

	new_child_node_ptr->child_node = create_ast_node(child_node_type);

	//插入结点到尾部，由于使用了头结点，尾插法较容易实现
	child_node_ptr *p = father->child_list;
	while (NULL != p->next) {
		p = p->next;
	}
	p->next = new_child_node_ptr;

	return new_child_node_ptr->child_node;
}

int show_ast(ast *root) {
	int spa_num = 0;//输出时候的空格数
	ast_node *p = root->root;
	show_node(p, 0);
	return 1;
}

int show_node(ast_node *node, int space_num){
	int i;
	child_node_ptr *p = node->child_list->next;
	for (i = 0; i < space_num; i++) {
		printf("| ");
	}
	if (node->node_type >= 258) {
		printf("%s\n", type_form[node->node_type - 258]);
	}
	else {
		printf("%c\n", node->node_type);
	}
	
	while(NULL != p) {
		show_node(p->child_node, space_num + 1);
		p = p->next;
	}
	return 1;
}

/*声明创建语法树相关的函数 end*/


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
ast_node *program() {
	ast_node *new_program = create_ast_node(PROGRAM);

	while ((EOF != tok) && (0 != tok)) {
		//现在有个问题，是在函数外面建立相应结点还是在函数内(内！！
		external_declaration(new_program);
	}
	return new_program;
}

//external_declaration
//	: type declarator decl_or_stmt
//	;
ast_node *external_declaration(ast_node *root) {

	ast_node *new_ex_decl = add_child_node_ptr(root, EXTERNAL_DECLARATION);
	type(new_ex_decl);
	declarator(new_ex_decl);
	decl_or_stmt(new_ex_decl);

	return new_ex_decl;
}

//decl_or_stmt
//	: '{' statement_list '}'
//	| '{' '}'
//	| ',' declarator_list ';'
//	| ';'
//	;
ast_node *decl_or_stmt(ast_node *root) {
	ast_node *new_DOS = add_child_node_ptr(root, DECL_OR_STMT);
	if ('{' == tok) {
		//add_child_node_ptr(new_DOS, tok);
		advance();
		if ('}' != tok) {
			statement_list(new_DOS);
			if ('}' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_DOS, tok);
				advance();
			}
		}
		else {
			//add_child_node_ptr(new_DOS, tok);
			advance();
		}
	}
	else if (',' == tok) {
		//add_child_node_ptr(new_DOS, tok);
		advance();
		declarator_list(new_DOS);
		if (';' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_DOS, tok);
			advance();
		}
	}
	else if (';' == tok) {
		//add_child_node_ptr(new_DOS, tok);
		advance();
	}
	else {
		find_error();
	}

	return new_DOS;
}

//declarator_list
//	: declarator
//	| declarator_list ',' declarator
//	;
ast_node *declarator_list(ast_node *root) {
	ast_node *new_decl_list = add_child_node_ptr(root, DECLARATOR_LIST);
	declarator(new_decl_list);
	while (',' == tok) {
		//add_child_node_ptr(new_decl_list, tok);
		advance();
		declarator(new_decl_list);
	}

	return new_decl_list;
}

//intstr_list
//	: initializer
//	| intstr_list ',' initializer
//	;
ast_node *intstr_list(ast_node *root) {
	ast_node *new_ins_list = add_child_node_ptr(root, INTSTR_LIST);
	initializer(new_ins_list);
	while (',' == tok) {
		//add_child_node_ptr(new_ins_list, tok);
		advance();
		initializer(new_ins_list);
	}

	return new_ins_list;
}

//initializer
//	: NUMBER
//	| STRING
//	;
ast_node *initializer(ast_node *root) {
	ast_node *new_init = add_child_node_ptr(root, INITIALIZER);
	if ((NUMBER != tok) && (STRING != tok)) {
		find_error();
	}
	else {
		//add_child_node_ptr(new_init, tok);
		advance();
	}

	return new_init;
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
ast_node *declarator(ast_node *root) {
	ast_node *new_decl = add_child_node_ptr(root, DECLARATOR);
	if (ID != tok) {
		find_error();
	}
	else {
		//add_child_node_ptr(new_decl, tok);
		advance();
		if ('=' == tok) {
			//add_child_node_ptr(new_decl, tok);
			advance();
			expr(new_decl);
		}
		else if ('(' == tok) {
			//add_child_node_ptr(new_decl, tok);
			advance();
			if (')' != tok) {
				parameter_list(new_decl);
			}
			if (')' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_decl, tok);
				advance();
			}
		}
		else if ('[' == tok) {
			//add_child_node_ptr(new_decl, tok);
			advance();
			if (']' == tok) {
				//add_child_node_ptr(new_decl, tok);
				advance();
				if ('=' == tok) {
					//add_child_node_ptr(new_decl, tok);
					advance();
					if ('{' != tok) {
						find_error();
					}
					else {
						//add_child_node_ptr(new_decl, tok);
						intstr_list(new_decl);
						if ('}' != tok) {
							find_error();
						}
						else {
							//add_child_node_ptr(new_decl, tok);
							advance();
						}
					}
				}
			}
			else {
				//(new_decl, tok);
				expr(new_decl);
				if (']' != tok) {
					find_error();
				}
				else {
					//add_child_node_ptr(new_decl, tok);
					if ('=' == tok) {
						//add_child_node_ptr(new_decl, tok);
						advance();
						if ('{' != tok) {
							find_error();
						}
						else {
							//add_child_node_ptr(new_decl, tok);
							intstr_list(new_decl);
							if ('}' != tok) {
								find_error();
							}
							else {
								//add_child_node_ptr(new_decl, tok);
								advance();
							}
						}
					}
				}
			}
		}
	}

	return new_decl;
}

//parameter_list
//	: parameter
//	| parameter_list ',' parameter
//	;
ast_node *parameter_list(ast_node *root) {
	ast_node *new_para_list = add_child_node_ptr(root, PARAMETER_LIST);
	parameter(new_para_list);
	while (',' == tok) {
		//add_child_node_ptr(new_para_list, tok);
		advance();
		parameter(new_para_list);
	}
	return new_para_list;
}

//parameter
//	: type ID
//	;
ast_node *parameter(ast_node *root) {
	ast_node *new_para = add_child_node_ptr(root, PARAMETER);
	type(new_para);
	if (ID != tok) {
		find_error();
	}
	else {
		//add_child_node_ptr(new_para, tok);
		advance();
	}
	return new_para;
}

//type
//	: INT
//	| STR
//	| VOID
//	;
ast_node *type(ast_node *root) {
	ast_node *new_type = add_child_node_ptr(root, TYPE);
	if ((INT != tok) && (STR != tok) && (VOID != tok)) {
		find_error();
	}
	else {
		//add_child_node_ptr(new_type, tok);
		advance();
	}
	return new_type;
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
ast_node *statement(ast_node *root) {
	ast_node *new_stmt = add_child_node_ptr(root, STATEMENT);
	if ('{' == tok) {
		//add_child_node_ptr(new_stmt, tok);
		advance();
		statement_list(new_stmt);
		if ('}' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_stmt, tok);
			advance();
		}
	}
	else if (IF == tok) {
		//add_child_node_ptr(new_stmt, tok);
		advance();
		if ('(' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_stmt, tok);
			advance();
			expr(new_stmt);
			if (')' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_stmt, tok);
				advance();
				statement(new_stmt);
				if (ELSE == tok) {
					//add_child_node_ptr(new_stmt, tok);
					advance();
					statement(new_stmt);
				}
			}
		}
	}
	else if (WHILE == tok) {
		//add_child_node_ptr(new_stmt, tok);
		advance();
		if ('(' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_stmt, tok);
			advance();
			expr(new_stmt);
			if (')' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_stmt, tok);
				advance();
				statement(new_stmt);
			}
		}
	}
	else if (RETURN == tok) {
		//add_child_node_ptr(new_stmt, tok);
		advance();
		if (';' == tok) {
			//add_child_node_ptr(new_stmt, tok);
			advance();
		}
		else {
			expr(new_stmt);
			if (';' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_stmt, tok);
				advance();
			}
		}
	}
	else if (PRINT == tok) {
		//add_child_node_ptr(new_stmt, tok);
		advance();
		if (';' == tok) {
			//add_child_node_ptr(new_stmt, tok);
			advance();
		}
		else {
			expr_list(new_stmt);
			if (';' != tok) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_stmt, tok);
				advance();
			}
		}
	}
	else if (SCAN == tok) {
		//add_child_node_ptr(new_stmt, tok);
		id_list(new_stmt);
		if (';' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_stmt, tok);
			advance();
		}
	}
	else if ((INT == tok) || (STR == tok) || (VOID == tok)) {
		type(new_stmt);
		declarator_list(new_stmt);
		if (';' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_stmt, tok);
			advance();
		}
	}
	else {
		expression_statement(new_stmt);
	}
	
	return new_stmt;
}

//statement_list
//	: statement
//	| statement_list statement
//	;
ast_node *statement_list(ast_node *root) {
	ast_node *new_stmt_list = add_child_node_ptr(root, STATEMENT_LIST);
	statement(new_stmt_list);
	while ((EOF != tok) && ('}' != tok)) {
		statement(new_stmt_list);
	}
	/*if ('}' == tok) {
		advance();
	}*/
	return new_stmt_list;
}

//expression_statement
//	: ';'
//	| expr ';'
//	;
ast_node *expression_statement(ast_node *root) {
	ast_node *new_expr_stmt = add_child_node_ptr(root, EXPRESSION_STATEMENT);
	if (';' == tok) {
		//add_child_node_ptr(new_expr_stmt, tok);
		advance();
	}
	else {
		expr(new_expr_stmt);
		if (';' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_expr_stmt, tok);
			advance();
		}
	}
	return new_expr_stmt;
}

//expr
//	: cmp_expr
//	;
ast_node *expr(ast_node *root) {
	ast_node *new_expr = add_child_node_ptr(root, EXPR);
	cmp_expr(new_expr);
	return new_expr;
}

//cmp_expr
//	: add_expr
//	| cmp_expr CMP add_expr
//	;
ast_node *cmp_expr(ast_node *root) {
	ast_node *new_cmp_expr = add_child_node_ptr(root, CMP_EXPR);
	add_expr(new_cmp_expr);
	while (CMP == tok) {
		//add_child_node_ptr(new_cmp_expr, tok);
		advance();
		add_expr(new_cmp_expr);
	}
	return new_cmp_expr;
}

//add_expr
//	: mul_expr
//	| add_expr '+' mul_expr
//	| add_expr '-' mul_expr
//	;
ast_node *add_expr(ast_node *root) {
	ast_node *new_add_expr = add_child_node_ptr(root, ADD_EXPR);
	mul_expr(new_add_expr);
	while (('+' == tok) || ('-' == tok)) {
		add_child_node_ptr(new_add_expr, tok);
		advance();
		mul_expr(new_add_expr);
	}
	return new_add_expr;
}

//mul_expr
//	: primary_expr
//	| mul_expr '*' primary_expr
//	| mul_expr '/' primary_expr
//	| mul_expr '%' primary_expr
//	| '-' primary_expr
//	;
ast_node *mul_expr(ast_node *root) {
	ast_node *new_mul_expr = add_child_node_ptr(root, MUL_EXPR);
	if ('-' == tok) {
		add_child_node_ptr(new_mul_expr, tok);
		advance();
		primary_expr(new_mul_expr);
	}
	else {
		primary_expr(new_mul_expr);
		while (('*' == tok) || ('/' == tok) || ('%' == tok)) {
			add_child_node_ptr(new_mul_expr, tok);
			advance();
			primary_expr(new_mul_expr);
		}
	}
	return new_mul_expr;
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
ast_node *primary_expr(ast_node *root) {
	ast_node *new_prim_expr = add_child_node_ptr(root, PRIMARY_EXPR);
	switch (tok) {
	case ID:
		//add_child_node_ptr(new_prim_expr, tok);
		advance();
		if ('(' == tok) {
			//add_child_node_ptr(new_prim_expr, tok);
			advance();
			if (')' != tok)
				expr_list(new_prim_expr);
			if (')' == tok) {
				//add_child_node_ptr(new_prim_expr, tok);
				advance();
			}
		}
		else if ('=' == tok) {
			//add_child_node_ptr(new_prim_expr, tok);
			advance();
			expr(new_prim_expr);
		}
		else if ('[' == tok) {
			//add_child_node_ptr(new_prim_expr, tok);
			advance();
			expr(new_prim_expr);
			if (']' != tok) {
				find_error();
				//return 0;
			}
			else {
				//add_child_node_ptr(new_prim_expr, tok);
				advance();
				if ('=' != tok) {
					find_error();
				}
				else {
					//add_child_node_ptr(new_prim_expr, tok);
					advance();
					expr(new_prim_expr);
				}
			}
		}
		else if (ASSIGN == tok) {
			//add_child_node_ptr(new_prim_expr, tok);
			advance();
			expr(new_prim_expr);
		}
		break;
	case '(':
		//add_child_node_ptr(new_prim_expr, tok);
		advance();
		expr(new_prim_expr);
		if (')' != tok) {
			find_error();
		}
		else {
			//add_child_node_ptr(new_prim_expr, tok);
			advance();
		}
		break;
	case NUMBER:
		//add_child_node_ptr(new_prim_expr, tok);
		advance();
		break;
	case STRING:
		//add_child_node_ptr(new_prim_expr, tok);
		advance();
		break;
	default:
		find_error();
	}

	return new_prim_expr;
}

//expr_list
//	: expr
//	| expr_list ',' expr
//	;
ast_node *expr_list(ast_node *root) {
	ast_node *new_expr_list = add_child_node_ptr(root, EXPR_LIST);
	expr(new_expr_list);
	while (tok == ',') {
		//add_child_node_ptr(new_expr_list, tok);
		advance();
		expr(new_expr_list);
	}

	return new_expr_list;
}

//id_list
//	: ID
//	| id_list ',' ID
//	;
ast_node *id_list(ast_node *root) {
	ast_node *new_id_list = add_child_node_ptr(root, ID_LIST);
	if (tok != ID) {
		find_error();
	}
	else {
		//add_child_node_ptr(new_id_list, tok);
		advance();
		while (tok == ',') {
			//add_child_node_ptr(new_id_list, tok);
			advance();
			if (tok != ID) {
				find_error();
			}
			else {
				//add_child_node_ptr(new_id_list, tok);
				advance();
			}
		}
	}
	
	return new_id_list;
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
		//by catfee
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

int main(int argc, char **argv)																																											//licenced by catfee
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
	ast *myast = (ast*)malloc(sizeof(ast));
	myast->root= program();
	printf("anal finished!\n");
	show_ast(myast);
	//past rr = astExpr();
	//showAst(rr, 0);

	return 0;
}
