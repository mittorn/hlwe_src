
#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_

#include "ss_parser.h"
#include "ss_kernel.h"
#include "ss_error.h"

struct Script
{
	Script ( GlobalScope *g, Script *n, Script *p ) : next(n), previous(p)
	{
		funcs = new Functions;
		properties = new ScriptScope (g);
	};

	~Script()
	{
		if (funcs) delete funcs;
		if (properties) delete properties;
	};

	Functions	*funcs;
	ScriptScope	*properties;
	Script		*next;
	Script		*previous;
};

class CScriptMgr
{
	GlobalScope	globals;
	Script		*last;

public:

	CScriptMgr() : last(0) {};

	~CScriptMgr()
	{
		while (last)
		{
			Script *tmp = last;
			last = last->previous;

			delete tmp;
		}
	};

	Script* PasteScript (char *filename)
	{
		last = new Script (&globals, 0, last);

		if (last->previous)
			last->previous->next = last;

		Constants *constants = new Constants;
		StructData *structs = new StructData;

		constants->Put ("BAD_INVOKATION", Type (NULL, BAD_INVOKATION));//Declare "BAD_INVOKATION" constant
		CreateKernel (last->funcs);

		ParseFile (filename, &globals, last->properties, constants, last->funcs, structs);

		delete structs;
		delete constants;

		last->properties->EndRegistration();

		if (last->funcs->Completed())
		{
			last->funcs->EndRegistration();
		}
		else
		{
			ErrorMessage ("Unsatistfied forward declaration(s) found");
		}

		last->properties->Prepare();

		return last;
	};

	void UnlinkScript ( Script *scr )
	{
		if (scr->next)
		{
			scr->next->previous = scr->previous;
		}

		if (scr->previous)
		{
			scr->previous->next = scr->next;
		}

		if (scr == last)
		{
			last = scr->previous;
		}

		delete scr;
	};
};

Script* CreateScript ( char *filename );
void FreeScript ( Script *scr );
bool NoErrors ();

#endif