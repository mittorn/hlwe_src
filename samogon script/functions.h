/*===============================================================================================

	All stuff for function calls handling - scope of functions, operator "call" etc...

===============================================================================================*/

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "operators.h"
#include "scope.h"
#include "mymap.h"
#include "keywords.h"
#include <vector>
#include <algorithm>

//========================================
//Generic class for both: standart and
//user-defined functions
//========================================

class GenericFunction
{
public:

	int argc;

	char specification;

	GenericFunction (int args, char s) : specification (s), argc (args) {};

	virtual Scope* MyScope () = 0;

	virtual void AddBody ( Operator *b ) = 0;

	virtual bool Forward () = 0;

	virtual void PreExecute () = 0;

	virtual void PassArg ( unsigned int argi, Type argv ) = 0;

	virtual Type Execute () = 0;

	virtual bool Constant ( bool args_constant ) = 0;

	virtual ~GenericFunction() {};
};

//========================================
//This is just a named set of instructions
//========================================

class Function : public GenericFunction
{
	Operator *body;

	bool has_body;

	Scope *_s;

public:

	Function ( Scope *s, char spec ) : GenericFunction (0, spec), _s (s), body (0), has_body (0) {};

	~Function ()
	{
		if (has_body)
		{
			delete body;
		}

		delete _s;
	};

	Scope* MyScope ()
	{
		return _s;
	};

	void AddBody ( Operator *b )
	{
		body = b;
		has_body = true;
	};

	bool Forward ()
	{
		return !has_body;
	};

	void PreExecute ()
	{
		_s->Prepare();
	};

	void PassArg ( unsigned int argi, Type argv )
	{
		_s->GetVariable(argi + 1) = argv;
	};

	Type Execute ()
	{
		try
		{
			body->execute();
		}
		catch ( return_value *v )
		{
			Type result = v->value;
			delete v;
			_s->Clear();

			return result;
		}

		_s->Clear();
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;//We can't check if user-defined function is constant, but we can do it for standart functions
	};
};

//================================================
//And this is a container for all these named sets
//================================================

class Functions
{
	struct checker
	{
		bool OK;

		checker() : OK (true) {};

		void operator() ( GenericFunction* f )
		{
			if (OK && f && f->Forward())
			{
				OK = false;
			}
		};
	};

	MyMap<unsigned int>				names;
	std::vector<GenericFunction*>	ids;

public:

	Functions () : ids (COUNT_OF_METHODS)//reserve 7 slots for special methods
	{
		for (int i = 0; i < COUNT_OF_METHODS; ++i)
		{
			ids[i] = 0;
		}
	};

	~Functions ()
	{
		for (int i = 0; i < ids.size(); ++i)
		{
			if (ids[i])
			{
				delete ids[i];//space for 7 method reserved, but there can be no methods
			}
		}

		names.clear ();
	};

	void Declare ( char *name, GenericFunction *f )
	{
		for (int j = 0; j < COUNT_OF_METHODS; ++j)
		{
			if (!strcmp(name, methods[j]))
			{
				ids[j] = f;
				names[name] = j + 1;//what is this for?
				return;
			}
		}

		ids.push_back ( f );
		names[name] = ids.size ();
	};

	unsigned int GetId ( char *name )
	{
		return names[name];
	};

	void AddArgToFn ( unsigned int id )
	{
		ids[id - 1]->argc++;
	};

	void AddBodyToFn ( unsigned int id, Operator *body )
	{
		ids[id - 1]->AddBody (body);//To support forward declarations and recurrent calls, we declaring function just when reading it's name, then adding up the body
	};

	bool Completed ()//Check for unsatistfied forward declarations
	{
		checker check;

		check = std::for_each (ids.begin(), ids.end(), check);

		return check.OK;
	};

	void EndRegistration ()//Delete all c-str keys
	{
		names.clear ();
	};

	GenericFunction* GetFn ( unsigned int id )
	{
		return ids[id  - 1];
	};

	Scope* GetScope ( unsigned int id )//>:(((
	{
		return ids[id - 1]->MyScope();
	};
};

//==========================================================
//This is operator, that can be used as a part of expression
//==========================================================

class Call : public Operator
{
	struct checker
	{
		bool CONSTANT;

		checker() : CONSTANT(true) {};

		void operator() ( Operator *o )
		{
			if (CONSTANT && o && !o->constant()) CONSTANT = false;
		};
	};

	unsigned int	_id;
	Functions		*list;

	std::vector<Operator*> args;
	std::vector<Type> tmp;//la-la-la

public:

	Call ( Functions *l, char *name ) : list (l), _id (l->GetId(name)) {};

	~Call ()
	{
		utility::killer kill;

		std::for_each (args.begin(), args.end(), kill);
	};

	void AddArg ( Operator *a )
	{
		args.push_back (a);
		tmp.push_back (_ZERO);//Has same effect as resize
	};

	bool ArgsCompleted ()
	{
		return list->GetFn(_id)->argc == args.size();
	};

	Type execute()
	{
		GenericFunction *me = list->GetFn (_id);

		int i;

		for (i = 0; i < me->argc; ++i)//calculate all argument expressions and store >8(
		{
			tmp[i] = args[i]->execute();
		}

		me->PreExecute ();//allocate new scope to don't damage previous values of local vars of function (in case of recursion)

		for (i = 0; i < me->argc; ++i)//move stored args to new scope
		{
			me->PassArg (i, tmp[i]);
		}

		return me->Execute(); 
	};

	bool constant ()
	{
		checker check;

		check = std::for_each (args.begin(), args.end(), check);

		return list->GetFn(_id)->Constant (check.CONSTANT);
	};
};

#endif