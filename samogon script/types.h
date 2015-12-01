/*===============================================================================================

	Here declared different basic types. Each type can be casted to any other type, and all of
	this types must support the same operators.

	What types we want to support:

	- int
	- double
	- string
	- array
	- structure

================================================================================================*/

#ifndef _TYPES_H_
#define _TYPES_H_

#pragma warning(disable:4800)//conversion from any type to boolean

#include "string.h"
#include "structures.h"

#define _ZERO	Type ()

#define _TRUE	Type (&tru, INT)
#define _FALSE	_ZERO

enum
{
	INT		= FIRST_FREE_TYPE - 4,
	DOUBLE	= FIRST_FREE_TYPE - 3,
	STRING	= FIRST_FREE_TYPE - 2,
	ARRAY	= FIRST_FREE_TYPE - 1
};

struct Type
{
	short int my_type;

	union
	{
		int			intv;
		double		doublev;
		char*		strv;

		struct
		{
			unsigned int size;//god damn it! sizeof doesn't works with dynamic arrays
			Type *v;
		}
		vectv;
	};

	explicit Type () : my_type (INT), intv (0) {};

	Type ( const Type& t )
	{
		my_type = t.my_type;

		switch (my_type)
		{
		case INT:

			intv = t.intv;
			break;

		case DOUBLE:

			doublev = t.doublev;
			break;

		case STRING:
			{
				int l = strlen (t.strv);

				strv = new char[l + 1];
				strcpy (strv, t.strv);
				strv[l] = '\0';
			}
			break;

		default:
			{
				vectv.v = new Type[t.vectv.size];

				for (unsigned int i = 0; i < t.vectv.size; ++i)
				{
					vectv.v[i] = t.vectv.v[i];
				}

				vectv.size = t.vectv.size;
			}
			break;
		}
	};

	explicit Type ( void *data, short int t, unsigned int s = 0 ) : my_type (t)
	{
		switch (t)
		{
		case INT:

			intv = *((int*)data);
			break;

		case DOUBLE:

			doublev = *((double*)data);
			break;

		case STRING:
			{
				char* s = (char*)data;
				int l = strlen (s);

				strv = new char[l + 1];
				strcpy (strv, s);
				strv[l] = '\0';
			}
			break;

		default:
			{
				Type *v = (Type*)data;

				vectv.v = new Type[s];

				for (unsigned int i = 0; i < s; ++i)
				{
					vectv.v[i] = v[i];
				}

				vectv.size = s;
			}
			break;
		}
	};

	~Type ()
	{
		Clear();
	};

	void Clear ()
	{
		if (my_type == STRING)
		{
			delete [] strv;
		}

		else if (my_type >= ARRAY)
		{
			delete [] vectv.v;//But why delete[] works? Maybe, it doesn't works too??? Dulls!!!
		}
	};

	Type operator= ( const Type& t )
	{
		Clear();

		my_type = t.my_type;

		switch (my_type)
		{
		case INT:
			intv = t.intv;
			break;

		case DOUBLE:
			doublev = t.doublev;
			break;

		case STRING:
			{
				int l = strlen (t.strv);

				strv = new char[l + 1];
				strcpy (strv, t.strv);
				strv[l] = '\0';
			}
			break;

		default:
			{
				vectv.v = new Type[t.vectv.size];

				for (unsigned int i = 0; i < t.vectv.size; ++i)
				{
					vectv.v[i] = t.vectv.v[i];
				}

				vectv.size = t.vectv.size;
			}
			break;
		}

		return *this;
	};

	Type operator== ( const Type& t )
	{
		int tru = 1;

		switch (my_type)
		{
		case INT:
			{
				switch (t.my_type)
				{
				case INT:
					{
						if (intv == t.intv)
							return _TRUE;
						else
							return _FALSE;
					}

				case DOUBLE:
					{
						if (intv == t.doublev)
							return _TRUE;
						else
							return _FALSE;
					}

				default:
					{
						return _FALSE;//string, struct can't be equal to a number
					}
				}
			}

		case DOUBLE:
			{
				switch (t.my_type)
				{
				case INT:
					{
						if (doublev == t.intv)
							return _TRUE;
						else
							return _FALSE;
					}

				case DOUBLE:
					{
						if (doublev == t.doublev)
							return _TRUE;
						else
							return _FALSE;
					}

				default:
					{
						return _FALSE;
					}
				}
			}

		case STRING:
			{
				switch (t.my_type)
				{
				case STRING:
					{
						if (!strcmp(strv, t.strv))
							return _TRUE;
						else
							return _FALSE;
					}

				default:
					{
						return _FALSE;
					}
				}
			}

		case ARRAY:
			{
				return _FALSE;//we can't compare arrays
			}

		default:
			{
				if (t.my_type == my_type)
				{
					for (unsigned int i = 0; i < vectv.size; ++i)
					{
						if (!bool(vectv.v[i] == t.vectv.v[i]))
						{
							return _FALSE;
						}
					}

					return _TRUE;
				}
				else
					return _FALSE;
			}
		}

		return _FALSE;
	};

	operator bool ()
	{
		switch (my_type)
		{
		case INT:
			return bool(intv);

		case DOUBLE:
			return bool(doublev);

		case STRING:
			return bool(strlen(strv));

		case ARRAY:
			{
				if (vectv.size <= 0) return false;

				for (unsigned int i = 0; i < vectv.size; ++i)
				{
					if (bool(vectv.v[i]))
					{
						return true;
					}
				}

				return false;

			}
			break;

		default:
			{
				for (unsigned int i = 0; i < vectv.size; ++i)
				{
					if (bool(vectv.v[i]))
					{
						return true;
					}
				}

				return false;
			}
			break;
		}

		return false;
	};
};

#pragma warning(default:4800)

#endif