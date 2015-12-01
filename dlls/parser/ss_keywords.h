#ifndef _KEYWORDS_H_
#define _KEYWORDS_H_

#define COUNT_OF_TYPES	3

static char *special_types[COUNT_OF_TYPES] =
{
	{"entvars"},
	{"Vector"},
	{"trace_result"}
};

#define ENTVARS_T		FIRST_FREE_TYPE + 0
#define VECTOR_T		FIRST_FREE_TYPE + 1
#define TRACE_RESULT_T	FIRST_FREE_TYPE + 2


#define COUNT_OF_METHODS	7

static char *methods[COUNT_OF_METHODS] =
{
	{"SPAWNED"},
	{"TOUCHED"},
	{"THINKED"},
	{"DAMAGED"},
	{"USED"},
	{"SAVE"},
	{"LOAD"}
};

#define M_SPAWN		1
#define	M_TOUCH		2
#define	M_THINK		3
#define	M_DAMAGED	4
#define	M_USE		5
#define	M_SAVE		6
#define	M_LOAD		7


#define COUNT_OF_KEYWORDS	29

static char *keywords[COUNT_OF_KEYWORDS] =
{
	{"include"},	//add code of file to current code
	{"var"},		//declaration of local variable
	{"function"},	//declaration of function
	{"return"},		//terminating function
	{"break"},		//terminating cycle
	{"continue"},	//go to next iteration
	{"while"},		//cycle
	{"if"},			//branching
	{"else"},		//alternate branch
	{"for"},		//compact form of while
	{"and"},		//logic
	{"or"},			//logic
	{"eq"},			//equals
	{"neq"},		//not equals
	{"gt"},			//greater
	{"lt"},			//litter
	{"gteq"},		//greater or equals
	{"lteq"},		//litter or equals
	{"of"},			//field access
	{"in"},			//indexation
	{"struct"},		//declaration of combined type
	{"const"},		//declaration of named constant
	{"global"},		//declaration of variable that is global for entire SERVER
	{"property"},	//declaration of property of current scripted entity
	{"neg"},		//unary -
	{"not"},		//!
	{"inv"},		//~
	{"inc"},		//postfix++
	{"dec"}			//postfix--
};

#define		_KW_INCLUDE		keywords[0]
#define		_KW_VAR			keywords[1]
#define		_KW_FUNCTION	keywords[2]
#define		_KW_RETURN		keywords[3]
#define		_KW_BREAK		keywords[4]
#define		_KW_CONTINUE	keywords[5]
#define		_KW_WHILE		keywords[6]
#define		_KW_IF			keywords[7]
#define		_KW_ELSE		keywords[8]
#define		_KW_FOR			keywords[9]
#define		_KW_AND			keywords[10]
#define		_KW_OR			keywords[11]
#define		_KW_EQ			keywords[12]
#define		_KW_NEQ			keywords[13]
#define		_KW_GT			keywords[14]
#define		_KW_LT			keywords[15]
#define		_KW_GTEQ		keywords[16]
#define		_KW_LTEQ		keywords[17]
#define		_KW_OF			keywords[18]
#define		_KW_IN			keywords[19]
#define		_KW_STRUCT		keywords[20]
#define		_KW_CONST		keywords[21]
#define		_KW_GLOBAL		keywords[22]
#define		_KW_PROPERTY	keywords[23]
#define		_KW_NEG			keywords[24]
#define		_KW_NOT			keywords[25]
#define		_KW_INV			keywords[26]
#define		_KW_INC			keywords[27]
#define		_KW_DEC			keywords[28]

#define		_KW_BEFORE		"before"
#define		_KW_INSTEAD		"instead"

#define	SPECIFICATION_DEFAULT	0
#define	SPECIFICATION_INSTEAD	1
#define	SPECIFICATION_BEFORE	2

bool IsKW (char *token);

#endif