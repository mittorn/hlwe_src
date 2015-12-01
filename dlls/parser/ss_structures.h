/*================================================================================================
		Basic container for structures data: fields offset and ... nothing more!
================================================================================================*/

#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include "ss_keywords.h"
#include "ss_map.h"
#include <vector>
#include <algorithm>

#define FIRST_FREE_TYPE		8
#define FIRST_USER_TYPE		FIRST_FREE_TYPE + COUNT_OF_TYPES

struct field_data
{
	field_data () : offset (0), type (0) {};
	field_data (unsigned int o, short int t) : offset (o), type (t) {};

	unsigned	int offset;
	short		int type;
};

struct type_data
{
	type_data () : id (0), size (0) {};
	type_data ( short int i, unsigned int s ) : id (i), size (s) {};

	unsigned	int size;
	short		int id;
};

class StructData
{
	MyMap<field_data>	fields;//I had just switched my brains off, because I had no time to think
	MyMap<type_data>	types;

	short int free_type;

public:

	StructData() : free_type (FIRST_USER_TYPE) {};//reserve 3 slots for entvars, Vector, and TraceResult

	void Declare ( char *name )
	{
		for (int i = 0; i < COUNT_OF_TYPES; ++i)
		{
			if (!strcmp(name, special_types[i]))
			{
				types[name] = type_data(FIRST_FREE_TYPE + i, 0);
				return;
			}
		}

		types[name] = type_data(free_type++, 0);
	};

	bool Declared ( char *name )
	{
		if (types[name].id)
		{
			return true;
		}

		return false;
	};

	void AddField ( char *type, char *fieldname )
	{
		fields[fieldname] = field_data (types[type].size++, types[type].id);
	};

	bool FieldDeclared ( char *fieldname )
	{
		if (fields[fieldname].type)
		{
			return true;
		}

		return false;
	};

	type_data GetTypeInfo ( char *type )
	{
		return types[type];
	};

	field_data GetFieldInfo ( char *fieldname )
	{
		return fields[fieldname];
	};
};

#endif