/*===============================================================================================
								PARSER BY LLAPb (C) 2008
===============================================================================================*/

#include "stdafx.h"
#include "ctype.h"

inline bool ISALPHA ( int c )
{
	return isalpha(c) || c == '_';
}

inline bool ISALNUM ( int c )
{
	return isalnum(c) || c == '_';
}

inline bool ISSPACE ( int c )
{
	return isspace(c) || c == '\n';
}

#include "parser.h"
#include "error.h"

#include "types.h"
#include "kernel.h"

extern int errors;

//Special function: returns pointer to first non-space symbol in string.
//If it's fails, this function returning iterator 'end'.
char* SkipSpaces ( char *begin, char *end )
{
	char *iterator = begin;

	while (iterator < end)
	{
		if (!ISSPACE(*iterator))
		{
			return iterator;
		}

		++iterator;
	}

	return end;
}

enum
{
	RETURN	= 1,
	BREAK	= 2,
	CONTINUE= 4
};

int ParseInt ( char *begin, char *end, int op )
{
	char token[256];
	int tok_pos = 0;

	memset (token, 0, sizeof token);

	char *iterator = SkipSpaces (begin, end);

	while (true)
	{
		if (iterator >= end || ISSPACE(*iterator))
		{
			token[tok_pos] = '\0';
			break;
		}
		else if (isdigit(*iterator))
		{
			token[tok_pos++] = *(iterator++);
		}
		else
		{
			if (op == CONTINUE)
				ErrorMessage ("Argument of statement \"continue\" must be numeric constant");
			else if (op == BREAK)
				ErrorMessage ("Argument of statement \"break\" must be numeric constant");

			return -1;
		}
	}

	if (SkipSpaces (iterator, end) >= end)
	{
		return strtol (token, 0, 10);
	}
	else
	{
		ErrorMessage ("Only numeric constants (not expressions) allowed here!");

		return -1;
	}
}


//Returns typeid and size of new declared structure, by the way
//adds data about new type to StructData
type_data ParseFieldsDeclaration ( char *begin, char *end, char *type, StructData *str )
{
	if (errors) return type_data();

	bool only_spaces = true;
	bool begun = true;//?

	char *it = ++begin;//Points at opening braket
	char argn[255];
	int arg_pos = 0;

	memset (argn, 0, sizeof argn);

	while (true)
	{
		it = SkipSpaces (it, end);

		if (it >= end)
		{
			if (begun)
			{
				ErrorMessage("Declaration of empty structure type is not permitted");

				return type_data();
			}
			else
			{
				ErrorMessage("Unnecessary \";\" in declaration of structure type");

				return type_data();
			}
		}

		while (true)
		{
			if (ISALNUM(*it))
			{
				argn[arg_pos++] = *(it++);
				only_spaces = false;
			}
			else
			{
				argn[arg_pos] = '\0';
				arg_pos = 0;
				goto CYCLE_END;
			}
		}

		CYCLE_END:

		it = SkipSpaces (it, end);

		if (*it == ';')
		{
			if (!strlen(argn))
			{
				char msg[255];
				sprintf(msg, "non-alphabetic name of field of structure \"%s\"", type);

				ErrorMessage(msg);
				return type_data();
			}
			else if (only_spaces)
			{
				char msg[255];
				sprintf(msg, "empty field name in structure \"%s\"", type);

				ErrorMessage(msg);
				return type_data();
			}

			if (IsKW(argn))
			{
				ErrorMessage("Field name matched with one of ss keywords");

				return type_data();
			}

			if (str->FieldDeclared(argn))
			{
				ErrorMessage("All structures must not have fields of same name");

				return type_data();
			}

			only_spaces = true;
			str->AddField (type, argn);

			++it;
		}
		else if (*it == '}')
		{
			if (!only_spaces)
			{
				ErrorMessage ("Probably, you forgot \";\" after declaration of field, or typed \"var\" keyword before field name");

				return type_data();
			}

			else if (!str->GetTypeInfo(type).size)
			{
				ErrorMessage ("Declaration of empty structure type is not permitted");

				return type_data();
			}

			break;
		}
		else
		{
			ErrorMessage("WHAT THE FUCK???");

			return type_data();
		}
	}

	return str->GetTypeInfo (type);
}

//Returns offset and type id by name of field, and only by name.
//So, there must be no same fields in different structure types
field_data ParseField ( char *begin, char *end, StructData *str )
{
	if (errors) return field_data();

	char *iterator = SkipSpaces (begin, end);
	int tok_pos = 0;

	char token[255];
	memset (token, 0, sizeof token);

	while (true)
	{
		if (iterator >= end || ISSPACE(*iterator))
		{
			token[tok_pos] = '\0';
			tok_pos = 0;
			break;
		}

		token[tok_pos++] = *(iterator++);
	}

	if (SkipSpaces (iterator, end) < end)
	{
		ErrorMessage ("Left argument of operator \"of\" must be field name, not string or something");

		return field_data();
	}

	if (str->FieldDeclared (token))
	{
		return str->GetFieldInfo (token);
	}
	else
	{
		char msg[255];
		sprintf (msg, "Unknown field of structure: \"%s\"", token);

		ErrorMessage(msg);
		return field_data();
	}
}

//Expressions in brakets has maximal priority, so we count them as a single operand -
//we put iterator from open braket to closing braket at once
char* SkipBraketExpression ( char *begin, char *end, char open, char close )
{
	int level = 1;

	char *iterator = begin;//point at open braket

	while (level)
	{
		++iterator;

		if (iterator >= end)
		{
			if (level)
			{
				return 0;
			}

			return end;
		}
		else if (*iterator == open)
		{
			++level;
		}
		else if (*iterator == close)
		{
			--level;
		}
	}

	return iterator;//return pointer to closing braket
}

char* SkipConstantLiteral ( char *begin, char *end )
{
	char *iterator = begin;//point at starting "

	while (true)
	{
		++iterator;

		if (iterator >= end)
		{
			return 0;
		}
		else if (*iterator == '\\')
		{
			++iterator;//skip this symbol
		}
		else if (*iterator == '\"')
		{
			break;
		}
	}

	return iterator;//point at ending "
}


//Array fast initializer
Operator* ParseArray (char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str)
{
	if (errors) return 0;

	Initializer *init = new Initializer;

	char *iterator = ++begin;//WARNING!!! iterator begin was changed

	while (true)
	{
		iterator = SkipSpaces (iterator, end);

		if (*iterator == '(')
		{
			iterator = SkipBraketExpression(iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				delete init;

				return 0;
			}
			else if (iterator >= end)
			{
				ErrorMessage ("Probably, you forgot closing braket somewhere in array initializer");

				delete init;

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression(iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Too much opening \"<\"!");

				delete init;

				return 0;
			}
			else if (iterator >= end)
			{
				ErrorMessage ("Probably, you forgot closing \">\" somewhere in array initializer");

				delete init;

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral(iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				delete init;

				return 0;
			}
			else if (iterator >= end)
			{
				ErrorMessage ("Probably, you forgot closing \" somewhere in array initializer");

				delete init;

				return 0;
			}
		}
		else if (*iterator == ',' || *iterator == '>')
		{
			init->add ( ParseExpression(begin, iterator, s, con, f, str) );

			if (*iterator == ',')
			{
				begin = ++iterator;//Warning!!! again
				continue;
			}
			else
			{
				break;
			}
		}

		++iterator;
	}

	return init;
}


//But when we got an expression, that is fully between the brakets, we just deleting this brakets
void StripBrakets ( char **begin, char **end )
{
	char *forward_iterator = *begin;
	char *backward_iterator = *end - 1;

	if (*(forward_iterator = SkipSpaces(*begin, *end)) == '(')
	{
		while (backward_iterator > forward_iterator)
		{
			if (!ISSPACE(*backward_iterator))
			{
				if (*backward_iterator == ')')
				{
					break;
				}
				else
				{
					return;
				}
			}

			--backward_iterator;
		}

		char *check_iterator = SkipBraketExpression (forward_iterator, backward_iterator + 1, '(', ')');

		if (check_iterator == backward_iterator)
		{
			*begin = forward_iterator + 1;
			*end = backward_iterator;
		}
	}
}


enum
{
	LV_ASSIGN,
	LV_OF,
	LV_IN,
	LV_INC,
	LV_DEC
};


//Special function for l-values parsing
LValue* ParseLValue ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str, int op )
{
	if (errors) return 0;

	char token[255];
	int tok_pos = 0;

	bool token_finished = false;

	StripBrakets  (&begin, &end);

	//Field access operator "of" and indexation operator "in"

	char *iterator = SkipSpaces(begin, end);

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '<')
			{
				iterator = SkipBraketExpression (iterator, end, '<', '>');

				if (!iterator)
				{
					ErrorMessage ("Unfinished initializer of array");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')//If we skipped all brakets, where is this from???
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_OF))
		{
			field_data info = ParseField (begin, iterator - 2, str);

			return new Of ( info.type, info.offset, ParseLValue (iterator, end, s, con, f, str, LV_OF) );
		}
		else if (!strcmp (token, _KW_IN))
		{
			return new In ( ParseExpression(begin, iterator - 2, s, con, f, str), ParseLValue (iterator, end, s, con, f, str, LV_IN) );
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	iterator = SkipSpaces(begin, end);

	if (iterator < end)
	{
		memset (token, 0, sizeof token);
		tok_pos = 0;

		while (true)
		{
			if (iterator >= end)
			{
				token[tok_pos] = '\0';
				token_finished = true;
				break;
			}
			else if (ISALNUM(*iterator))
			{
				token[tok_pos++] = *(iterator++);
			}
			else
			{
				token[tok_pos] = '\0';
				break;
			}
		}

		if (token_finished)
		{
			if (Scope *scope = s->Declared (token))
			{
				return new Variable (scope, token);
			}

			char msg[255];

			sprintf (msg, "Undeclared identifier: \"%s\"", token);
			ErrorMessage (msg);

			return 0;
		}
		else
		{
			if ((iterator = SkipSpaces(iterator, end)) >= end)
			{
				if (Scope *scope = s->Declared (token))
				{
					return new Variable (scope, token);
				}

				char msg[255];

				sprintf (msg, "Undeclared identifier: \"%s\"", token);
				ErrorMessage (msg);

				return 0;
			}
			else
			{
				if (op == LV_ASSIGN)
					ErrorMessage ("Left operand of operator \"=\" must be lvlue!!!");
				else if (op == LV_OF)
					ErrorMessage ("Right operand of operator \"of\" must be lvlue!!!");
				else if (op == LV_IN)
					ErrorMessage ("Right operand of operator \"in\" must be lvlue!!!");
				else if (op == LV_INC)
					ErrorMessage ("Right operand of operator \"inc\" must be lvlue!!!");
				else if (op == LV_DEC)
					ErrorMessage ("Right operand of operator \"dec\" must be lvlue!!!");

				return 0;
			}
		}
	}

	ErrorMessage ("Found operator without operands!");

	return 0;
}


/*==================================== PARSING PRIORITY: ========================================

  	- ( )
	- or
	- and
	- eq, neq, gt, lt, gteq, lteq
	- =
	- +, -
	- *, /, %
	- |
	- &
	- of, in
	- inc, dec, not, neg, inv
	- constant literals, lvalues, function calls

*/

//Recursive parsing of expressions in order of priority
Operator* ParseExpression ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str )
{
	if (errors) return 0;

	char token[1024];
	int tok_pos;

	bool token_finished;
	bool token_isnumeric;
	bool token_isfloat;
	bool token_isstr;
	bool token_isarr;

	char *iterator;

	StripBrakets (&begin, &end);

	iterator = SkipSpaces (begin, end);

	if (iterator >= end)
	{
		ErrorMessage ("Found operators without operands!");

		return 0;
	}

	//Try to find operator with maximal  priority: logic "or"

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')//If we skipped all brakets, where is this from???
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_OR))
		{
			Operator *result = new Or ( ParseExpression (begin, iterator-2, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	//Logic "and"

	iterator = SkipSpaces (begin, end);

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '<')
			{
				iterator = SkipBraketExpression (iterator, end, '<', '>');

				if (!iterator)
				{
					ErrorMessage ("Unfinished initializer of array");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_AND))
		{
			Operator *result = new And ( ParseExpression (begin, iterator-3, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;		
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	//Comparison operators - all they have the same priority

	iterator = SkipSpaces (begin, end);

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '<')
			{
				iterator = SkipBraketExpression (iterator, end, '<', '>');

				if (!iterator)
				{
					ErrorMessage ("Unfinished initializer of array");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_EQ))
		{
			Operator* result = new Eq ( ParseExpression (begin, iterator-2, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_NEQ))
		{
			Operator* result = new Neq ( ParseExpression (begin, iterator-3, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_GT))
		{
			Operator* result = new Gt ( ParseExpression (begin, iterator-2, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_LT))
		{
			Operator* result = new Lt ( ParseExpression (begin, iterator-2, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_GTEQ))
		{
			Operator* result = new Gt_or_eq ( ParseExpression (begin, iterator-4, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_LTEQ))
		{
			Operator* result = new Lt_or_eq ( ParseExpression (begin, iterator-4, s, con, f, str), ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	//Try to parse assignment operation

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		if (iterator >= end)
		{
			break;
		}

		else if (*iterator == '(')
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
		}
		else if (*iterator == ')')
		{
			ErrorMessage ("Too much closing brakets!");

			return 0;
		}

		else if (*iterator == '=')
		{
			return new Assignment( ParseLValue(begin, iterator, s, con, f, str, LV_ASSIGN), ParseExpression(iterator + 1, end, s, con, f, str) );
		}

		++iterator;
	}

	//Try to parse summ or diff

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		if (iterator >= end)
		{
			break;
		}

		else if (*iterator == '(')
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
		}
		else if (*iterator == ')')
		{
			ErrorMessage ("Too much closing brakets!");

			return 0;
		}

		else if (*iterator == '+')
		{
			Operator *result = new Summ( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		else if (*iterator == '-')
		{
			Operator *result = new Diff( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		++iterator;
	}

	//Try to parse mult or div

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		if (iterator >= end)
		{
			break;
		}

		else if (*iterator == '(')
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
		}
		else if (*iterator == ')')
		{
			ErrorMessage ("Too much closing brakets!");

			return 0;
		}

		else if (*iterator == '*')
		{
			Operator *result = new Mult( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		else if (*iterator == '/')
		{
			Operator *result = new Div( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		else if (*iterator == '%')
		{
			Operator *result = new Mod( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		++iterator;
	}

	//Try to parse bitset merging

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		if (iterator >= end)
		{
			break;
		}

		else if (*iterator == '(')
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
		}
		else if (*iterator == ')')
		{
			ErrorMessage ("Too much closing brakets!");

			return 0;
		}

		else if (*iterator == '|')
		{
			Operator *result = new Merge( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		++iterator;
	}

	//Try to parse intersection of bitsets

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		if (iterator >= end)
		{
			break;
		}

		else if (*iterator == '(')
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');

			if (!iterator)
			{
				ErrorMessage ("Too much opening brakets!");

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
		}
		else if (*iterator == ')')
		{
			ErrorMessage ("Too much closing brakets!");

			return 0;
		}

		else if (*iterator == '&')
		{
			Operator *result = new Intersect( ParseExpression(begin, iterator, s, con, f, str), ParseExpression(iterator + 1, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		++iterator;
	}

	//Field access operator "of" and indexation operator "in"

	iterator = SkipSpaces(begin, end);

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '<')
			{
				iterator = SkipBraketExpression (iterator, end, '<', '>');

				if (!iterator)
				{
					ErrorMessage ("Unfinished initializer of array");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')//If we skipped all brakets, where is this from???
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_OF))
		{
			field_data info = ParseField (begin, iterator - 2, str);

			Operator *result = new Of ( info.type, info.offset, ParseLValue (iterator, end, s, con, f, str, LV_OF) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_IN))
		{
			Operator *result = new In ( ParseExpression(begin, iterator - 2, s, con, f, str), ParseLValue(iterator, end, s, con, f, str, LV_IN) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	//At last, parse unary operators. All they have the same priority

	iterator = SkipSpaces (begin, end);

	while (true)
	{
		tok_pos = 0;
		memset (token, 0, sizeof token);

		while (true)
		{
			if (iterator >= end || ISSPACE(*iterator))
			{
				token[tok_pos] = '\0';
				tok_pos = 0;
				break;
			}
			else if (*iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Too much opening brakets!");

					return 0;
				}
			}
			else if (*iterator == '<')
			{
				iterator = SkipBraketExpression (iterator, end, '<', '>');

				if (!iterator)
				{
					ErrorMessage ("Unfinished initializer of array");

					return 0;
				}
			}
			else if (*iterator == '\"')
			{
				iterator = SkipConstantLiteral (iterator, end);

				if (!iterator)
				{
					ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

					return 0;
				}
			}
			else if (*iterator == ')')
			{
				ErrorMessage ("Too much closing brakets!");

				return 0;
			}

			token[tok_pos++] = *(iterator++);
		}

		if (!strcmp (token, _KW_NEG))
		{
			Operator* result = new Neg ( ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_NOT))
		{
			Operator* result = new Not ( ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_INV))
		{
			Operator* result = new Inv ( ParseExpression (iterator, end, s, con, f, str) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_INC))
		{
			Operator* result = new Inc ( ParseLValue (iterator, end, s, con, f, str, LV_INC) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}
		else if (!strcmp (token, _KW_DEC))
		{
			Operator* result = new Dec ( ParseLValue (iterator, end, s, con, f, str, LV_DEC) );

			if (!errors && result->constant())//in case of error we can get access violation
			{
				Type value = result->execute();
				delete result;

				result = new Constant( value );
			}

			return result;
		}

		iterator = SkipSpaces (iterator, end);

		if (iterator >= end)
		{
			break;
		}
	}

	//Now try to parse OPERANDS. Lowest level expressions: function calls, variables, constants

	tok_pos = 0;
	memset (token, 0, sizeof token);

	Operator *arraytok = 0;

	token_finished = false;
	token_isnumeric = true;
	token_isfloat = false;
	token_isstr = false;
	token_isarr = false;

	iterator = SkipSpaces (begin, end);

	if (*iterator == '\"')
	{
		token_isstr = true;
		token_isnumeric = false;
		//token_finished will allways equal to false

		while (true)
		{
			++iterator;

			if (iterator >= end)
			{
				ErrorMessage ("Unterminated string literal. If you want to use symbol \" inside of string, use sequence \\\"");

				return 0;
			}
			else if (*iterator == '\\')
			{
				++iterator;//skip this symbol
				token[tok_pos++] = *iterator;
			}
			else if (*iterator == '\"')
			{
				token[tok_pos] = '\0';
				break;
			}
			else
			{
				token[tok_pos++] = *iterator;
			}
		}

		++iterator;
	}
	else if (*iterator == '<')
	{
		token_isarr = true;
		token_isnumeric = false;
		//token_finished will allways equal to false

		char *array_begin = iterator;

		iterator = SkipBraketExpression (array_begin, end, '<', '>');

		if (!iterator || iterator >= end)
		{
			ErrorMessage ("Probably, you forgot \">\"");

			return 0;
		}
		else if (SkipSpaces (++iterator, end) < end)
		{
			ErrorMessage ("Probably, you forgot \";\"");

			return 0;
		}

		arraytok = ParseArray (array_begin, iterator, s, con, f, str);
	}
	else
	{
		while (true)
		{
			if (iterator >= end)
			{
				token[tok_pos] = '\0';
				token_finished = true;
				break;
			}
			else if (ISALNUM(*iterator))
			{
				if (ISALPHA(*iterator))
				{
					token_isnumeric = false;
					token_isfloat = false;
				}

				token[tok_pos++] = *(iterator++);
			}
			else if (*iterator == '.')
			{
				token[tok_pos++] = *(iterator++);
				if (token_isnumeric) token_isfloat = true;
			}
			else
			{
				token[tok_pos] = '\0';
				break;
			}
		}
	}

	if (token_finished)
	{
		if (token_isfloat)
		{
			double fv = strtod(token, 0);

			return new Constant ( Type(&fv, DOUBLE) );
		}
		else if (token_isnumeric)
		{
			int iv = strtol(token, 0, 10);
			return new Constant ( Type(&iv, INT) );
		}
		else if (Scope *scope = s->Declared(token))
		{
			return new Variable (scope, token);
		}
		else if (con->Declared(token))
		{
			return new Constant (con->Get(token));
		}

		char msg[255];

		sprintf (msg, "Undeclared identifier: \"%s\"", token);
		ErrorMessage (msg);

		return 0;
	}
	else
	{
		iterator = SkipSpaces(iterator, end);

		if (iterator >= end)
		{
			if (token_isfloat)
			{
				double fv = strtod(token, 0);

				return new Constant ( Type(&fv, DOUBLE) );
			}
			else if (token_isnumeric)
			{
				int iv = strtol(token, 0, 10);

				return new Constant ( Type(&iv, INT) );
			}
			else if (token_isstr)
			{
				return new Constant ( Type(token, STRING) );
			}
			else if (token_isarr)
			{
				return arraytok;
			}
			else if (Scope *scope = s->Declared(token))
			{
				return new Variable (scope, token);
			}
			else if (con->Declared(token))
			{
				return new Constant (con->Get(token));
			}

			char msg[255];

			sprintf (msg, "Undeclared identifier: \"%s\"", token);
			ErrorMessage (msg);

			return 0;
		}
		else
		{
			if (*iterator == '(')//This is call of function. Parse all parameters etc...
			{
				for (int j = 0; j < COUNT_OF_METHODS; ++j)
				{
					if (!strcmp(methods[j], token))
					{
						char msg[255];
						sprintf(msg, "Illegal call of special method \"%s\" from script", token);

						ErrorMessage(msg);
						return 0;
					}
				}

				if (f->GetId(token))
				{
					Call *c = new Call (f, token);

					char *args_end = SkipBraketExpression(iterator, end, '(', ')');

					if (!args_end)
					{
						ErrorMessage ("Probably, you forgot to close arguments list");

						delete c;

						return 0;
					}

					else if (SkipSpaces (args_end + 1, end) != end )
					{
						ErrorMessage ("Probably, it's too much closing brakets in code");

						delete c;

						return 0;
					}

					char *args_begin = ++iterator;

					if (c->ArgsCompleted() && SkipSpaces (args_begin, args_end) == args_end)
					{
						return c;
					}
					else
					{
						while (true)
						{
							iterator = SkipSpaces (iterator, args_end);

							if (*iterator == '(')
							{
								iterator = SkipBraketExpression(iterator, args_end, '(', ')');

								if (!iterator)
								{
									ErrorMessage ("Too much opening brakets!");

									delete c;

									return 0;
								}
							}
							else if (*iterator == '<')
							{
								iterator = SkipBraketExpression(iterator, end, '<', '>');

								if (!iterator)
								{
									ErrorMessage ("Too much opening \"<\"!");

									delete c;

									return 0;
								}
								else if (iterator >= end)
								{
									ErrorMessage ("Probably, you forgot closing \">\" somewhere in arguments list of function");

									delete c;

									return 0;
								}
							}
							else if (*iterator == '\"')
							{
								iterator = SkipConstantLiteral(iterator, args_end);

								if (!iterator)
								{
									ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

									delete c;

									return 0;
								}
							}
							else if (*iterator == ',' || *iterator == ')')
							{
								c->AddArg ( ParseExpression(args_begin, iterator, s, con, f, str) );

								if (*iterator == ',')
								{
									args_begin = ++iterator;
									continue;
								}
								else
								{
									break;
								}
							}

							++iterator;
						}

						if (c->ArgsCompleted())
						{
							return c;
						}
						else
						{
							char msg[255];

							sprintf (msg, "Function \"%s\" must have %i parameters", token, f->GetFn(f->GetId(token))->argc);
							ErrorMessage (msg);

							delete c;

							return 0;
						}
					}
				}
				else
				{
					char msg[255];

					sprintf (msg, "Undeclared function: \"%s\"", token);
					ErrorMessage (msg);

					return 0;
				}
			}
			else
			{
				char msg[255];

				sprintf (msg, "Idiot! What did you done with token \"%s\"???", token);
				ErrorMessage (msg);

				if (arraytok) delete arraytok;

				return 0;
			}
		}
	}

	ErrorMessage ("What the fuck???");

	return 0;
}

//====================================== WHAT TO PARSE: =========================================
/*

		- "{" -> "}" - combined operator
		- "var" - register variable in  current scope (probably, it needs initialization)
		- "if", "for" and "while"
		- "return" - register exception-throwing operator
		- "break", "continue" - register exception-throwing operator if it is permitted
		- and register simple expressions by default

*/

Operator* ParseOperator ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str, int permitted )
{
	if (errors) return 0;

	char token[255];
	int tok_pos;

	char *iterator;

	memset (token, 0, sizeof token);
	tok_pos = 0;

	iterator = SkipSpaces (begin, end);

	if ( iterator >= end )
	{
		ErrorMessage ("Empty operator found!");

		return 0;
	}

	if ( *iterator == '{' )
	/*================================
	"{" ------> "}"
	================================*/
	{
		return ParseCombinedOperator (iterator, end, s, con, f, str, permitted);
	}

	//if it fails, try to get token:
	while (true)
	{
		if (!ISALNUM(*iterator) || iterator >= end)
		{
			token[tok_pos] = '\0';
			tok_pos = 0;

			break;
		}

		token[tok_pos++] = *(iterator++);
	}

	if (!strcmp(token, _KW_VAR))
	/*================================
	"var" -> name -> EoLN
			  |
			 "=" -> expression
	================================*/
	{
		if ((iterator = SkipSpaces (iterator, end)) >= end)
		{
			ErrorMessage ("Declaration of unnamed variable? Are you crazy?");

			return 0;
		}
		else
		{
			memset (token, 0, sizeof token);

			char *expr_begin = iterator;

			while (true)
			{
				if (!ISALNUM(*iterator) || iterator >= end)
				{
					token[tok_pos] = '\0';
					tok_pos = 0;

					break;
				}

				token[tok_pos++] = *(iterator++);
			}

			if (s->Declared (token) || con->Declared (token))
			{
				char errmsg[255];
				sprintf (errmsg, "\"%s\": redefinition.", token);
				ErrorMessage (errmsg);

				return 0;
			}
			else
			{
				if (!IsKW(token))
				{
					s->RegisterVariable (token);
				}
				else
				{
					ErrorMessage("Variable name matched with one of ss keywords");

					return 0;
				}
			}

			if ((iterator = SkipSpaces (iterator, end)) != end)
			{
				if (*iterator == '=')
				{
					return ParseExpression (expr_begin, end, s, con, f, str);
				}
				else
				{
					ErrorMessage ("What is this shit???");

					return 0;
				}
			}
			else
			{
				return 0;//It is not an error, we will just add nothing
			}
		}
	}

	else if (!strcmp(token, _KW_RETURN))
	/*=================================
	"return" -> expression
	=================================*/
	{
		if ( permitted & RETURN )
		{
			return new Return ( ParseExpression (iterator, end, s, con, f, str) );
		}
		else
		{
			ErrorMessage ("operator \"return\" is not permitted here");

			return 0;
		}
	}

	else if (!strcmp(token, _KW_CONTINUE))
	/*=================================
	"continue" -> numeric_constant
	=================================*/
	{
		if ( permitted & CONTINUE )
		{
			return new Continue ( ParseInt (iterator, end, CONTINUE) );
		}
		else
		{
			ErrorMessage ("operator \"continue\" is not permitted here");

			return 0;
		}
	}

	else if (!strcmp(token, _KW_BREAK))
	/*=================================
	"break" -> numeric_constant
	=================================*/
	{
		if ( permitted & BREAK )
		{
			return new Break ( ParseInt (iterator, end, BREAK) );
		}
		else
		{
			ErrorMessage ("operator \"break\" is not permitted here");

			return 0;
		}
	}

	else if (!strcmp(token, _KW_WHILE))
	/*=============================================
	"while" -> "(" -> expression -> ")" -> operator
									 |
								    EoLN
	=============================================*/
	{
		Scope *scope = new ScriptScope (s);
		While *w = new While (scope);

		iterator = SkipSpaces (iterator, end);

		if ( *iterator != '(' )
		{
			ErrorMessage ("Found operator \"while\" without predicate");

			delete w;

			return 0;
		}
		else
		{
			char *start = iterator;

			iterator = SkipBraketExpression (start, end, '(', ')');

			if ( !iterator )
			{
				ErrorMessage ("Not enough of closing brakets (\")\") in predicate of operator \"while\"");

				delete w;

				return 0;
			}
			
			w->add_pred ( ParseExpression (start + 1, iterator, scope, con, f, str) );

			iterator = SkipSpaces (++iterator, end);

			if ( iterator < end )
			{
				w->add_body ( ParseOperator (iterator, end, scope, con, f, str, permitted | CONTINUE | BREAK ) );
			}
		}

		scope->EndRegistration ();

		return w;
	}

	else if (!strcmp(token, _KW_IF))
	/*===================================================
	"if" -> "(" -> expression -> ")" -> operator -> EoLN
											|
					  EoLN <- operator <- "else"
	===================================================*/
	{
		Scope *scope = new ScriptScope (s);
		If *i = new If (scope);

		iterator = SkipSpaces (iterator, end);

		if ( *iterator != '(' )
		{
			ErrorMessage ("Found operator \"if\" without predicate");

			delete i;

			return 0;
		}
		else
		{
			char *start = iterator;

			iterator = SkipBraketExpression (start, end, '(', ')');

			if ( !iterator )
			{
				ErrorMessage ("Not enough of closing brakets (\")\") in predicate of operator \"if\"");

				delete i;

				return 0;
			}
			
			i->add_pred ( ParseExpression (start + 1, iterator, scope, con, f, str) );

			iterator = SkipSpaces (++iterator, end);

			//Try to find "else"
			start = iterator;
			tok_pos = 0;
			memset (token, 0, sizeof token);

			while (true)
			{
				iterator = SkipSpaces (iterator, end);

				while (true)
				{
					if (iterator >= end)
					{
						token[tok_pos] = '\0';
						tok_pos = 0;

						break;
					}
					else if ( *iterator == '{' )
					{
						iterator = SkipBraketExpression (iterator, end, '{', '}');//"else" can be inside of "{ }"

						if ( !iterator )
						{
							ErrorMessage ("Too much opening brakets! (Did you forgot\"}\"?)");

							delete i;

							return 0;
						}
						else if ( iterator >= end )
						{
							ErrorMessage ("Probably, you forgot \"}\"");

							delete i;

							return 0;
						}
					}
					else if ( *iterator == '\"' )
					{
						iterator = SkipConstantLiteral (iterator, end);

						if ( !iterator )
						{
							ErrorMessage ("Found unterminated string");

							delete i;

							return 0;
						}
						else if ( iterator >= end )
						{
							ErrorMessage ("Probably, you forgot closing \"");

							delete i;

							return 0;
						}
					}
					else if (ISSPACE(*iterator) || iterator >= end)
					{
						token[tok_pos] = '\0';
						tok_pos = 0;

						break;
					}

					token[tok_pos++] = *(iterator++);
				}

				if ( !strcmp(token, _KW_ELSE) )
				{
					i->add_top ( ParseOperator (start, iterator - 5 /*length of else*/, scope, con, f, str, permitted) );
					i->add_bottom ( ParseOperator (iterator, end, scope, con, f, str, permitted) );

					break;
				}

				if ( iterator >= end )
				{
					i->add_top ( ParseOperator (start, iterator, scope, con, f, str, permitted) );

					break;
				}
			}
		}

		scope->EndRegistration ();

		return i;
	}

	else if (!strcmp(token, _KW_FOR))
	/*=======================================================================================================
	"for" -> "(" -> expression|NOTHING -> ";" -> expression|NOTHING -> ";" -> expression|NOTHING -> ")"
																									 |
																							 operator|NOTHING
	=======================================================================================================*/
	{
		Scope *scope = new ScriptScope (s);
		For *fr = new For (scope);

		iterator = SkipSpaces (iterator, end);

		if ( *iterator != '(' )
		{
			ErrorMessage ("Found operator \"for\" without predicate");

			delete fr;

			return 0;
		}
		else
		{
			char *start = ++iterator;
			bool empty = true;

			while (true)
			{
				if (*iterator == ';')
				{
					break;
				}

				if (*iterator == '(')
				{
					iterator = SkipBraketExpression (iterator, end, '(', ')');
				}

				else if (*iterator == ')')
				{
					ErrorMessage ("If you want to skip some of expressions in predicate of operator \"for\", use following syntax: for ( ; ; )");

					delete fr;

					return 0;
				}

				if (!ISSPACE(*iterator))
				{
					empty = false;
				}

				++iterator;
			}

			if (!empty)
			{
				fr->add_init ( ParseExpression (start, iterator, scope, con, f, str) );
			}

			empty = true;
			start = ++iterator;

			while (true)
			{
				if (*iterator == ';')
				{
					break;
				}

				if (*iterator == '(')
				{
					iterator = SkipBraketExpression (iterator, end, '(', ')');
				}

				else if (*iterator == ')')
				{
					ErrorMessage ("If you want to skip some of expressions in predicate of operator \"for\", use following syntax: for ( ; ; )");

					delete fr;

					return 0;
				}

				if (!ISSPACE(*iterator))
				{
					empty = false;
				}

				++iterator;
			}

			if (!empty)
			{
				fr->add_check ( ParseExpression (start, iterator, scope, con, f, str) );
			}

			empty = true;
			start = ++iterator;

			while (true)
			{
				if (*iterator == '(')
				{
					iterator = SkipBraketExpression (iterator, end, '(', ')') + 1;
				}

				else if (*iterator == ')')
				{
					break;
				}

				if (*iterator == ';')
				{
					ErrorMessage ("Too much expressions in predicate of operator \"for\". Maximal 3 supported");

					delete fr;

					return 0;
				}

				if (!ISSPACE(*iterator))
				{
					empty = false;
				}

				++iterator;
			}

			if (!empty)
			{
				fr->add_inc ( ParseExpression (start, iterator, scope, con, f, str) );
			}

			start = SkipSpaces (++iterator, end);

			if (start < end)
			{
				fr->add_body ( ParseOperator (start, end, scope, con, f, str, permitted | CONTINUE | BREAK) );
			}
		}

		return fr;
	}

	/*==============================
	   Standart errors reportings
	==============================*/

	else if (!strcmp(token, _KW_ELSE))
	{
		ErrorMessage ("Found \"else\" without matching \"if\"");

		return  0;
	}
	else if (!strcmp(token, _KW_STRUCT))
	{
		ErrorMessage ("Structure type declaration is not permitted inside of function");

		return  0;
	}
	else if (!strcmp(token, _KW_FUNCTION))
	{
		ErrorMessage ("Declaration of function inside of function is not permitted");

		return  0;
	}
	else if (!strcmp(token, _KW_GLOBAL) || !strcmp(token, _KW_PROPERTY))
	{
		ErrorMessage ("Specifiers \"global\" and \"property\" are permitted only in global scope");

		return  0;
	}
	else if (!strcmp(token, _KW_INCLUDE))
	{
		ErrorMessage ("Include directive is not permitted inside of function");

		return  0;
	}
	else if (!strcmp(token, _KW_CONST))
	{
		ErrorMessage ("Declaration of constants is permitted only outside of function");

		return  0;
	}

	return ParseExpression (begin, end, s, con, f, str);//By default
}

//Iterative parsing of declared variables and operators calling
Operator* ParseCombinedOperator ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str, int permitted )
{
	if (errors) return 0;

	if (*begin != '{')
	{
		ErrorMessage ("You forgot \"{\" in declaration of function");

		return 0;
	}

	char *iterator = ++begin;//WARNING!!! Iterator begin was changed

	Scope *scope = new ScriptScope (s);
	Combined *body = new Combined ( scope );

	bool only_spaces = true;

	while (true)
	{
		if ( *iterator == '{' )
		{
			iterator = SkipBraketExpression (iterator, end, '{', '}');//";" can be inside of "{ }"
			only_spaces = false;

			if ( !iterator )
			{
				ErrorMessage ("Too much opening brakets! (Did you forgot\"}\"?)");

				delete body;

				return  0;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot \";\"");

				delete body;

				return 0;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);
			only_spaces = false;

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				delete body;

				return 0;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot closing \"");

				delete body;

				return 0;
			}
		}
		else if ( *iterator == '(' )
		{
			iterator = SkipBraketExpression (iterator, end, '(', ')');// for(a;b;c) !!!
			only_spaces = false;

			if ( !iterator )
			{
				ErrorMessage ("Too much opening brakets! (Did you forgot\")\"?)");

				delete body;

				return  0;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot \";\"");

				delete body;

				return 0;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				delete body;

				return 0;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot \">\"");

				delete body;

				return 0;
			}
		}
		else if ( *iterator == '}' )
		{
			++iterator;

			if ( SkipSpaces (iterator, end) >= end && only_spaces )
			{
				break;
			}
			else
			{
				ErrorMessage ("Probably, you forgot \";\"");

				delete body;

				return 0;
			}
		}
		else if ( *iterator == ';' )//so, we must separate even operator "if" from others by ";"
		{							//but there must not be ";" before "else"!!!
			Operator *o = ParseOperator (begin, iterator, scope, con, f, str, permitted);

			if (o)
			{
				body->add (o);
			}//o can be equals to 0, in case if we parsed declaration of new variable

			begin = ++iterator;//WARNING again
			only_spaces = true;

			continue;
		}
		//????

		++iterator;
	}

	scope->EndRegistration ();

	return body;
}


//=================== PARSE DECLARATIONS OF FUNCTIONS AND GLOBAL VARIABLES ======================
//====================== And, by the way, includes and structured types =========================

	/*

		//First of all, variables:

			1) find statement "var" and get name of variable etc

			2) if "=" follows name, parse expression and check if it is constant
			(Global variables can be initialized only by constant expressions)

		//Functions:

			1) find statement "function", get it's name and arguments
			and check if it was already declared

			2) if it wasn't, declare it at once! (and set number of args for it)

			3) if it was, check if it was just forward declaration

			4) in this case check if number of arguments is equal to number of arguments
			in forward declaration

			5) if everything is finally fine, get scope of this function
			and parse body of function as a combined operator with this scope

		//Includes...

		//Structures...

	*/

void ParseDeclaration ( char *begin, char *end, Scope *g, Scope *s, Constants *con, Functions *f, StructData *str )
{
	if (errors) return;

	char token[255];
	int tok_pos;

	char *iterator;

	memset (token, 0, sizeof token);
	tok_pos = 0;

	iterator = SkipSpaces (begin, end);

	while (true)
	{
		if (!ISALNUM(*iterator) || iterator >= end)
		{
			token[tok_pos] = '\0';
			tok_pos = 0;

			break;
		}

		token[tok_pos++] = *(iterator++);
	}

	if (!strcmp(token, _KW_INCLUDE))
	/*=============================
	 include -> " -> filename -> "
	=============================*/
	{
		iterator = SkipSpaces(iterator, end);

		if (iterator >= end)
		{
			ErrorMessage("Empty include statement");

			return;
		}

		else if (*iterator != '"')
		{
			ErrorMessage("Correct syntaxis of include statement is: include \"file.txt\";");

			return;
		}

		char buffer[255];
		char buf_pos  = 0;
		char *fbegin = ++iterator;

		while (*iterator != '"')
		{
			if (iterator >= end)
			{
				ErrorMessage("You forgot terminating \" in include statement");

				return;
			}

			buffer[buf_pos++] = *(iterator++);
		}

		buffer[buf_pos] = '\0';

		if (SkipSpaces(iterator+1, end) < end)
		{
			ErrorMessage("Unterminated include statement");

			return;
		}

		ParseFile (buffer, g, s, con, f, str);
	}

	else if (!strcmp(token, _KW_CONST))
	/*======================================
	"const" -> name -> EoLN
				  |
				 "=" -> constant_expression
	======================================*/
	{
		if ((iterator = SkipSpaces (iterator, end)) >= end)
		{
			ErrorMessage ("Declaration of unnamed constant? Are you crazy?");

			return;
		}
		else
		{
			memset (token, 0, sizeof token);

			while (true)
			{
				if (!ISALNUM(*iterator) || iterator >= end)
				{
					token[tok_pos] = '\0';
					tok_pos = 0;

					break;
				}

				token[tok_pos++] = *(iterator++);
			}

			if (s->Declared (token) || con->Declared (token))
			{
				char errmsg[255];
				sprintf (errmsg, "\"%s\": redefinition.", token);
				ErrorMessage (errmsg);

				return;
			}
			else if (IsKW(token))
			{
				ErrorMessage("Constant name matched with one of ss keywords");

				return;
			}

			if ((iterator = SkipSpaces (iterator, end)) != end)
			{
				if (*iterator == '=')
				{
					Operator *v = ParseExpression(iterator+1, end, s, con, f, str);

					if (!errors && v->constant())
					{
						con->Put (token, v->execute());
						delete v;
					}
					else
					{
						ErrorMessage ("Constant might be initialized only with constant expression!");

						delete v;

						return;
					}
				}
				else
				{
					ErrorMessage ("What is this shit???");

					return;
				}
			}
			else
			{
				char errmsg[255];
				sprintf (errmsg, "Constant \"%s\" must be initialized straight after declaration.", token);
				ErrorMessage (errmsg);

				return;
			}
		}
	}

	else if (!strcmp(token, _KW_GLOBAL))
	/*==================================
	"global" -> name -> EoLN
				  |
				 "=" -> error!
	==================================*/
	{
		if ((iterator = SkipSpaces (iterator, end)) >= end)
		{
			ErrorMessage ("Declaration of unnamed global variable? Are you crazy?");

			return;
		}
		else
		{
			memset (token, 0, sizeof token);

			while (true)
			{
				if (!ISALNUM(*iterator) || iterator >= end)
				{
					token[tok_pos] = '\0';
					tok_pos = 0;

					break;
				}

				token[tok_pos++] = *(iterator++);
			}

 			if (s->Declared (token, true) || con->Declared (token))
			{
				char errmsg[255];
				sprintf (errmsg, "\"%s\": redefinition.", token);
				ErrorMessage (errmsg);

				return;
			}
			else
			{
				if (!IsKW(token))
				{
					g->RegisterVariable (token);
				}
				else
				{
					ErrorMessage("Global variable name matched with one of ss keywords");

					return;
				}
			}

			if ((iterator = SkipSpaces (iterator, end)) != end)
			{
				if (*iterator == '=')
				{
					ErrorMessage ("Initialization of global variables is not permitted");
				}
				else
				{
					ErrorMessage ("What is this shit???");

					return;
				}
			}
			else
			{
				return;
			}
		}
	}

	else if (!strcmp(token, _KW_PROPERTY))
	/*==================================
	"property" -> name -> EoLN
				  |
				 "=" -> error!
	==================================*/
	{
		if ((iterator = SkipSpaces (iterator, end)) >= end)
		{
			ErrorMessage ("Declaration of unnamed property? Are you crazy?");

			return;
		}
		else
		{
			memset (token, 0, sizeof token);

			while (true)
			{
				if (!ISALNUM(*iterator) || iterator >= end)
				{
					token[tok_pos] = '\0';
					tok_pos = 0;

					break;
				}

				token[tok_pos++] = *(iterator++);
			}

			if (s->Declared (token) || con->Declared(token))
			{
				char errmsg[255];
				sprintf (errmsg, "\"%s\": redefinition.", token);
				ErrorMessage (errmsg);

				return;
			}
			else
			{
				if (!IsKW(token))
				{
					s->RegisterVariable (token);
				}
				else
				{
					ErrorMessage("Property name matched with one of ss keywords");

					return;
				}
			}

			if ((iterator = SkipSpaces (iterator, end)) != end)
			{
				if (*iterator == '=')
				{
					ErrorMessage ("Initialization of properties is not permitted");
				}
				else
				{
					ErrorMessage ("What is this shit???");

					return;
				}
			}
			else
			{
				return;
			}
		}
	}

	else if (!strcmp(token, _KW_FUNCTION))
	/*==================================================
	"function" -> NAME -> "(" -> args... -> ")" -> EoLN
		 |			^						 |
		 -> directive  combined_operator <- "{"
	==================================================*/
	{
		iterator = SkipSpaces ( iterator, end );

		char specification = SPECIFICATION_DEFAULT;

		//Now try to check if directive is specified
		if (*iterator == '<')
		{
			char *dir_begin = iterator + 1;

			char *dir_end = SkipBraketExpression(dir_begin, end, '<', '>');

			if (!dir_end || dir_end >= end)
			{
				ErrorMessage ("Error in directive specification");

				return;
			}

			char *specific = new char[dir_end - dir_begin + 1];

			strncpy (specific, dir_begin, dir_end - dir_begin);

			specific[dir_end - dir_begin] = '\0';

			if (!strcmp(specific, _KW_BEFORE))
			{
				specification = SPECIFICATION_BEFORE;
			}
			else if (!strcmp(specific, _KW_INSTEAD))
			{
				specification = SPECIFICATION_INSTEAD;
			}
			else
			{
				ErrorMessage ("Unknown directive");

				delete [] specific;

				return;
			}

			delete [] specific;

			iterator = SkipSpaces (dir_end + 1, end);

			if (!iterator || iterator >= end)
			{
				ErrorMessage ("Error in directive specification");

				return;
			}
		}

		char name[255];

		memset (name, 0, sizeof name);
		int nam_pos = 0;

		while (true)
		{
			if (!ISALNUM(*iterator) || iterator >= end)
			{
				name[nam_pos] = '\0';
				break;
			}

			name[nam_pos++] = *(iterator++);
		}

		if (!strlen(name))
		{
			ErrorMessage ("Function must have a name");

			return;
		}

		if (IsKW(name))
		{
			ErrorMessage ("Function name matched with one of ss keywords");

			return;
		}

		unsigned int id = f->GetId(name);

		if (!id)
		{
			Function *func = new Function ( new ScriptScope (s), specification );
			f->Declare (name, func);
			id = f->GetId(name);

			iterator = SkipSpaces (iterator, end);
			char *args_begin = iterator + 1;//After opening braket
			char *args_end;

			if (iterator < end && *iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Probably, you forgot to close arguments list of function");

					return;
				}
				else if ( iterator >= end )
				{
					ErrorMessage ("???");

					return;
				}

				args_end = iterator;//At close breaket

				//Ok, it's forward declaration without body
				if ((iterator = SkipSpaces (args_end + 1, end)) >= end)//Now just count arguments...
				{
					if (SkipSpaces (args_begin, args_end) < args_end)
					{
						while (args_begin != args_end)
						{
							if (*args_begin == ',')
							{
								f->AddArgToFn (id);
							}

							++args_begin;
						}

						f->AddArgToFn (id);
					}
				}
				else//It is full declaration of function
				{
					bool only_spaces = true;
					bool begun = true;//?

					char *it = args_begin;
					char argn[255];
					int arg_pos = 0;

					memset (argn, 0, sizeof argn);

					while (true)
					{
						it = SkipSpaces (it, args_end);

						if (it == args_end)
						{
							if (begun)
							{
								break;
							}
							else
							{
								ErrorMessage("Unnecessary \",\" in declaration of function");

								return;
							}
						}

						while (true)
						{
							if (ISALNUM(*it))
							{
								argn[arg_pos++] = *(it++);
								only_spaces = false;
							}
							else
							{
								argn[arg_pos] = '\0';
								arg_pos= 0;
								goto OUTSIDE;
							}
						}

						OUTSIDE:

						it = SkipSpaces (it, args_end);

						if (*it == ')' || *it == ',')
						{
							if (!strlen(argn))
							{
								char msg[255];
								sprintf(msg, "non-alphabetic name of parameter in function \"%s\"", name);

								ErrorMessage(msg);
								return;
							}

							else if (only_spaces)
							{
								char msg[255];
								sprintf(msg, "empty parameter in function \"%s\"", name);

								ErrorMessage(msg);
								return;
							}

							only_spaces = true;
							f->AddArgToFn(id);

							Scope *sc = f->GetScope(id);

							if (sc->Declared(argn) || con->Declared(argn))
							{
								char msg[255];
								sprintf(msg, "Redefinition of parameter \"%s\" in function \"%s\"", argn, name);

								ErrorMessage(msg);
								return;
							}
							else
							{
								if (!IsKW(argn))
								{
									sc->RegisterVariable (argn);
								}
								else
								{
									ErrorMessage("Parameter name matched with one of ss keywords");

									return;
								}
							}

							if (*it == ')')
							{
								break;
							}

							++it;
						}
						else
						{
							ErrorMessage("Fantastic! You are idiot!");

							return;
						}
					}

					f->AddBodyToFn (id, ParseCombinedOperator (iterator, end, f->GetScope(id), con, f, str, RETURN));
				}
			}
			else
			{
				char msg[255];
				sprintf (msg, "Missing arguments list in declaration of function \"%s\"", name);

				ErrorMessage (msg);

				return;
			}
		}
		else if (f->GetFn(id)->Forward())
		{
			iterator = SkipSpaces (iterator, end);
			char *args_begin = iterator + 1;//After opening braket
			char *args_end;

			if (iterator < end && *iterator == '(')
			{
				iterator = SkipBraketExpression (iterator, end, '(', ')');

				if (!iterator)
				{
					ErrorMessage ("Probably, you forgot to close arguments list of function");

					return;
				}
				else if ( iterator >= end )
				{
					ErrorMessage ("???");

					return;
				}

				args_end = iterator;//At close breaket

				//Attempting to make another forward declaration?
				if ((iterator = SkipSpaces (args_end + 1, end)) >= end)
				{
					ErrorMessage ("Multiple forward declarations are not permitted");
					return;
				}
				else//Satistfaction of forward declaration
				{
					int argc = 0;
					bool only_spaces = true;
					bool begun = true;//?

					char *it = args_begin;
					char argn[255];
					int arg_pos = 0;

					memset (argn, 0, sizeof argn);

					while (true)
					{
						it = SkipSpaces (it, args_end);

						if (it == args_end)
						{
							if (!f->GetFn(id)->argc && begun)
							{
								break;
							}
							else
							{
								char msg[255];
								sprintf(msg, "Number of parameters in function \"%s\" mismatching with forward declaration", name);

								ErrorMessage(msg);
								return;
							}
						}

						while (true)
						{
							if (ISALNUM(*it))
							{
								argn[arg_pos++] = *(it++);
								only_spaces = false;
							}
							else
							{
								argn[arg_pos] = '\0';
								arg_pos= 0;
								goto EXTERN;
							}
						}

						EXTERN:

						it = SkipSpaces (it, args_end);

						if (*it == ')' || *it == ',')
						{
							if (!strlen(argn))
							{
								char msg[255];
								sprintf(msg, "non-alphabetic name of parameter in function \"%s\"", name);

								ErrorMessage(msg);
								return;
							}

							else if (only_spaces)
							{
								char msg[255];
								sprintf(msg, "empty parameter in function \"%s\"", name);

								ErrorMessage(msg);
								return;
							}

							only_spaces = true;
							++argc;

							Scope *sc = f->GetScope(id);

							if (sc->Declared(argn) || con->Declared(argn))
							{
								char msg[255];
								sprintf(msg, "Redefinition of parameter \"%s\" in function \"%s\"", argn, name);

								ErrorMessage(msg);
								return;
							}
							else
							{
								if (!IsKW(argn))
								{
									sc->RegisterVariable (argn);
								}
								else
								{
									ErrorMessage("Parameter name matched with one of ss keywords");

									return;
								}
							}

							if (*it == ')')
							{
								if (f->GetFn(id)->argc == argc)
								{
									break;
								}
								else
								{
									char msg[255];
									sprintf(msg, "function \"%s\" must have %i parameters", name, f->GetFn(id)->argc);

									ErrorMessage(msg);
									return;
								}
							}

							++it;
						}
						else
						{
							ErrorMessage("Fantastic! You are idiot!");

							return;
						}
					}

					f->AddBodyToFn (id, ParseCombinedOperator (iterator, end, f->GetScope(id), con, f, str, RETURN));
				}
			}
			else
			{
				char msg[255];
				sprintf (msg, "Missing arguments list in declaration of function \"%s\"", name);

				ErrorMessage (msg);

				return;
			}
		}
		else
		{
			ErrorMessage ("Multiple forward declarations are not permitted");

			return;
		}
	}

	else if (!strcmp(token, _KW_STRUCT))
	/*================================================
	"struct" -> name -> "{" -> fields... -> ";" -> "}"
						 |
					  nothing ->  ERROR!
	================================================*/
	{
		char name[255];
		int nam_pos = 0;

		memset (name, 0, sizeof name);

		iterator = SkipSpaces (iterator, end);

		if (!iterator)
		{
			ErrorMessage ("Declaration of unnamed structure type? What the fuck???");

			return;
		}

		while (true)
		{
			if (!ISALNUM(*iterator) || iterator >= end)
			{
				name[nam_pos] = '\0';
				break;
			}

			name[nam_pos++] = *(iterator++);
		}

		if (!strlen(name))
		{
			ErrorMessage ("Structure type must have a name");

			return;
		}

		if (IsKW(name))
		{
			ErrorMessage ("Structure type name matched with one of ss keywords");

			return;
		}

		if (f->GetId(name))
		{
			char msg[1024];
			sprintf (msg, "Function with name \"%s\" already declared, so constructor of structure type \"%s\" can't be created. Rename function or structure type", name, name);

			ErrorMessage(msg);
			return;
		}
		else if (str->Declared(name))
		{
			char msg[1024];
			sprintf (msg, "Type \"%s\": redefinition", name);

			ErrorMessage(msg);
			return;
		}

		char *body_begin = SkipSpaces (iterator, end);

		if (!body_begin)
		{
			ErrorMessage ("Declaration of structure without fields? It's not permitted!");

			return;
		}

		if (*body_begin != '{')
		{
			char msg[255];
			sprintf (msg, "Missing fields list in declaration of structure type \"%s\"", name);

			ErrorMessage (msg);
			return;
		}

		iterator = SkipBraketExpression (body_begin, end, '{', '}');

		if (!iterator || iterator >= end)
		{
			ErrorMessage ("Probably, you forgot closing braket \"}\" in declaration of structure");

			return;
		}

		if (SkipSpaces (++iterator, end) < end)
		{
			ErrorMessage ("Probably, you forgot \";\" after declaration of structure type");

			return;
		}

		str->Declare (name);

		type_data info = ParseFieldsDeclaration (body_begin, iterator, name, str);

		f->Declare ( name, new Constructor (info.id, info.size) );
	}

	else
	{
		char msg[255];
		sprintf (msg, "Unknown token in global scope: \"%s\"", token);

		ErrorMessage (msg);
	}
}

void ParseText ( char *begin, char *end, Scope *g, Scope *s, Constants *con, Functions *f, StructData *str )
{
	if (errors) return;

	char *iterator = begin;
	bool only_spaces = true;

	while (true)
	{
		if ( iterator >= end )
		{
			if ( only_spaces )
			{
				break;
			}
			else
			{
				ErrorMessage ("Unexpected end of file. Didn't you forgot \";\" after function's body?");

				return;
			}
		}
		else if ( *iterator == '{' )
		{
			iterator = SkipBraketExpression (iterator, end, '{', '}');//";" can be inside of "{ }"
			only_spaces = false;

			if ( !iterator )
			{
				ErrorMessage ("Too much opening brakets! (Did you forgot\"}\"?)");

				return;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot \"}\"");

				return;
			}
		}
		else if (*iterator == '<')
		{
			iterator = SkipBraketExpression (iterator, end, '<', '>');

			if (!iterator)
			{
				ErrorMessage ("Unfinished initializer of array");

				return;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot \">\"");

				return;
			}
		}
		else if (*iterator == '\"')
		{
			iterator = SkipConstantLiteral (iterator, end);
			only_spaces = false;

			if (!iterator)
			{
				ErrorMessage ("Found unterminated string. If you want to use symbol \" inside of string, use sequence \\\"");

				return;
			}
			else if ( iterator >= end )
			{
				ErrorMessage ("Probably, you forgot closing \"");

				return;
			}
		}
		else if ( *iterator == ';' )
		{
			ParseDeclaration (begin, iterator, g, s, con, f, str);

			begin = ++iterator;//WARNING! iterator begin was changed
			only_spaces = true;

			continue;
		}

		if (!ISSPACE(*iterator))
		{
			only_spaces = false;
		}

		++iterator;
	}
}

//======================== PARSE FILE CONTENT AND CUT COMMENTS  =================================

void ParseFile ( char *fname, Scope *g, Scope *s, Constants *con, Functions *f, StructData *str )
{
	if (errors) return;

	unsigned int fsize = 0;
	FILE *fifo;//:)))
	char *main_expr, buffer[2];
	int comment_type = 0;// 0- code, 1- c++ comment, 2- c comment

	fifo = fopen(fname, "r");

	if (!fifo)
	{
		char msg[255];
		sprintf(msg, "file \"%s\" not found", fname);

		ErrorMessage(msg);
		return;
	}

	while (!feof(fifo))
	{
		fgetc(fifo);
		++fsize;
	}

	rewind(fifo);

	main_expr = new char[fsize];

	fsize = 0;

	while (!feof(fifo))
	{
		if (comment_type == 0)
		{
			main_expr[fsize] = fgetc(fifo);

			if (fsize > 0)
			{
				if (main_expr[fsize] == '*' && main_expr[fsize - 1] == '/')//c-style comment
				{
					comment_type = 2;
					main_expr[fsize - 1] = ' ';//comment is equal to whitespace
					--fsize;
					buffer[0] = buffer[1] = '\0';
				}
				else if (main_expr[fsize] == '/' && main_expr[fsize - 1] == '/')//c++ comment
				{
					comment_type = 1;
					main_expr[fsize - 1] = ' ';
					--fsize;
					buffer[0] = buffer[1] = '\0';
				}
			}

			++fsize;
		}
		else if (comment_type == 1)
		{
			buffer[0] = fgetc(fifo);

			if (buffer[0] == '\n')
			{
				comment_type = 0;
			}
		}
		else if (comment_type == 2)
		{
			buffer[0] = buffer[1];

			buffer[1] = fgetc(fifo);

			if (buffer[1] == '/' && buffer[0] == '*')
			{
				comment_type = 0;
			}
		}
	}

	fclose(fifo);

	ParseText (main_expr, main_expr + fsize - 1, g, s, con, f, str);

	delete [] main_expr;
}