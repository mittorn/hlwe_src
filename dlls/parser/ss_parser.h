/*===============================================================================================
					Here defined all functions that are needed for parsing
===============================================================================================*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include "ss_consttable.h"
#include "ss_functions.h"

Operator* ParseExpression ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str );

Operator* ParseCombinedOperator ( char *begin, char *end, Scope *s, Constants *con, Functions *f, StructData *str, int permitted );

void ParseText ( char *begin, char *end, Scope *g, Scope *s, Constants *con, Functions *f, StructData *str );

void ParseFile ( char *fname, Scope *g, Scope *s, Constants *con, Functions *f, StructData *str );

#endif