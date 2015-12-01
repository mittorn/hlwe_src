/*===============================================================================================

	Here defined container for variables. It contains associative array for fast registration
	of vars by name and stack of vectors for fast access to variables during run-time

===============================================================================================*/

#ifndef _SCOPE_H_
#define _SCOPE_H_

#pragma warning(disable:4786) //too complicated template classes hieracy

#include "string.h"

#include "types.h"

#include "mystack.h"
#include "mymap.h"

#include <vector>
#include <algorithm>

class Scope
{
public:

	virtual ~Scope() {};
	virtual void RegisterVariable ( char *name )					= 0;
	virtual int GetId ( char *name )								= 0;
	virtual Type& GetVariable ( int id )							= 0;
	virtual void EndRegistration ()									= 0;
	virtual Scope* Declared ( char *name, bool to_declare = false )	= 0;//Returns scope, where variable with name "name" is declared
	virtual void Prepare ()											= 0;
	virtual void Clear ()											= 0;
};

class GlobalScope : public Scope
{
	MyMap<unsigned int>	names;
	std::vector<Type>	ids;

public:

	~GlobalScope ()
	{
		names.clear();
	};

	void RegisterVariable ( char *name )
	{
		if (!names[name])
		{
			ids.push_back(_ZERO);
			names[name] = ids.size();
		}
	};

	int GetId ( char *name )
	{
		return names[name];
	};

	Type& GetVariable ( int id )
	{
		return ids[id - 1];
	};

	void EndRegistration ()	{};//Never end registration - different scripts can be precached in different time

	Scope* Declared ( char *name, bool to_declare )
	{
		if (!to_declare && names[name])
		{
			return this;
		}

		return 0;//It allows multiple declarations of global variable. Bad side of this - is possibility to declare few globals with same name in the same script
	};

	void Prepare ()	{};//It's allways prepared

	void Clear ()	{};
};

class ScriptScope : public Scope
{
	typedef std::vector<Type> scope_t;

	Scope *superior;//Global scope for this scope
	MyMap<unsigned int> names;
	MyStack<scope_t> ids;
	unsigned int size;

public:

	ScriptScope ( Scope *s ) : superior (s), size (0) {};

	~ScriptScope ()
	{
		names.clear ();
	}

	void RegisterVariable ( char *name )
	{
		names[name] = ++size;
	};

	int GetId ( char *name )
	{
		return names[name];
	};

	Type& GetVariable ( int id )
	{
		return ids.top()[id - 1];
	};

	void EndRegistration ()
	{
		names.clear ();//Deletes all c-str keys
	};

	Scope* Declared ( char *name, bool to_declare )
	{
		if (names[name])
		{
			return this;
		}
		else if (superior)
		{
			return superior->Declared (name, to_declare);//trace up to find scope, where this variable is declared
		}

		return 0;
	};

	void Prepare ()
	{
		ids.push ( scope_t (size) );//Put vector of [size] elements on top of stack
	};

	void Clear ()
	{
		ids.pop ();//Windup
	};
};

#pragma warning(default:4786)

#endif