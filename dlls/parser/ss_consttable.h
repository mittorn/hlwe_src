#ifndef	_CONSTTABLE_H_
#define	_CONSTTABLE_H_

#include "ss_types.h"
#include "ss_map.h"
#include <vector>

class Constants
{
	MyMap<unsigned int> list;
	std::vector<Type> array;

public:

	bool Declared ( char *name )
	{
		if (list[name]) return true;

		return false;
	};

	void Put ( char *name, Type value )
	{
		array.push_back (value);
		list[name] = array.size();
	};

	Type Get ( char *name )
	{
		return array[list[name] - 1];
	};
};

#endif