#include "stdafx.h"
#include "operators.h"

#include <algorithm>

#include "math.h"

#pragma warning(disable:4800)//conversion from any type to boolean

Variable::Variable ( Scope *s, char *name ) : _s (s)
{
	_id = _s->GetId (name);
}

Type Variable::execute()
{
	return _s->GetVariable (_id);
}

Type& Variable::instance()
{
	return _s->GetVariable (_id);
}


Type Summ::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					int summ = l_t.intv + r_t.intv;//int + int

					return Type (&summ, INT);
				}

			case DOUBLE:
				{
					double summ = l_t.intv + r_t.doublev;//int + double -> cast to maximal sized type

					return Type (&summ, DOUBLE);
				}

			default:
				{
					return _ZERO;//int + struct, string -> has no sence
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double summ = l_t.doublev + r_t.intv;//double + int

					return Type (&summ, DOUBLE);
				}

			case DOUBLE:
				{
					double summ = l_t.doublev + r_t.doublev;//double + double

					return Type (&summ, DOUBLE);
				}

			default:
				{
					return _ZERO;//double + struct, string -> has no sence
				}
			}
		}

	case STRING:
		{
			switch (r_t.my_type)
			{
			case STRING:
				{
					char *concat = new char[strlen(l_t.strv) + strlen(r_t.strv) + 1];// }|{ECTb

					strncpy (concat, l_t.strv, strlen(l_t.strv));
					strncpy (concat + strlen(l_t.strv), r_t.strv, strlen(r_t.strv));

					concat[strlen(l_t.strv) + strlen(r_t.strv)] = '\0';

					Type tmp(concat, STRING);

					delete concat;
					return tmp;
				}

			default:
				{
					return _ZERO;//string + struct, int, double -> has no sence
				}
			}
		}

	default:
		{
			return _ZERO;//struct + struct -> has no sence
		}
	}
}


Type Diff::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					int diff = l_t.intv - r_t.intv;//int - int

					return Type (&diff, INT);
				}

			case DOUBLE:
				{
					double diff = l_t.intv - r_t.doublev;//int - double -> cast to maximal sized type

					return Type (&diff, DOUBLE);
				}

			default:
				{
					return _ZERO;//int - struct, string -> has no sence
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double diff = l_t.doublev - r_t.intv;//double - int

					return Type (&diff, DOUBLE);
				}

			case DOUBLE:
				{
					double diff = l_t.doublev - r_t.doublev;//double - double

					return Type (&diff, DOUBLE);
				}

			default:
				{
					return _ZERO;//double - struct, string -> has no sence
				}
			}
		}

	default:
		{
			return _ZERO;//struct, string - struct, string, int, double -> has no sence
		}
	}
}


Type Mult::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					int mult = l_t.intv * r_t.intv;//int * int

					return Type (&mult, INT);
				}

			case DOUBLE:
				{
					double mult = l_t.intv * r_t.doublev;//int * double -> cast to maximal sized type

					return Type (&mult, DOUBLE);
				}

			default:
				{
					return _ZERO;//int * struct, string -> has no sence
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double mult = l_t.doublev * r_t.intv;//double * int

					return Type (&mult, DOUBLE);
				}

			case DOUBLE:
				{
					double mult = l_t.doublev * r_t.doublev;//double * double

					return Type (&mult, DOUBLE);
				}

			default:
				{
					return _ZERO;//double * struct, string -> has no sence
				}
			}
		}

	default:
		{
			return _ZERO;//struct, string * struct, string, double, int -> has no sence
		}
	}
}


Type Div::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double div = double(l_t.intv) / r_t.intv;//int / int = double!!!

					return Type (&div, DOUBLE);
				}

			case DOUBLE:
				{
					double div = double(l_t.intv) / r_t.doublev;//int / double -> cast to maximal sized type

					return Type (&div, DOUBLE);
				}

			default:
				{
					return _ZERO;//int / struct, string -> has no sence
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double div = l_t.doublev / r_t.intv;//double / int

					return Type (&div, DOUBLE);
				}

			case DOUBLE:
				{
					double div = l_t.doublev / r_t.doublev;//double / double

					return Type (&div, DOUBLE);
				}

			default:
				{
					return _ZERO;//double / struct, string -> has no sence
				}
			}
		}

	default:
		{
			return _ZERO;//struct, string / struct, string, double, int -> has no sence
		}
	}
}


Type Mod::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					int mod = l_t.intv % r_t.intv;//int % int

					return Type (&mod, INT);
				}

			case DOUBLE:
				{
					double mod = fmod(l_t.intv, r_t.doublev);//int % double -> there can be fraction

					return Type (&mod, DOUBLE);
				}

			default:
				{
					return _ZERO;//int % struct, string -> has no sence
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					double mod = fmod(l_t.doublev, r_t.intv);//double % int -> there can be fraction

					return Type (&mod, DOUBLE);
				}

			case DOUBLE:
				{
					double mod = fmod(l_t.doublev, r_t.doublev);//double % double

					return Type (&mod, DOUBLE);
				}

			default:
				{
					return _ZERO;//double / struct, string -> has no sence
				}
			}
		}

	default:
		{
			return _ZERO;//struct, string / struct, string, double, int -> has no sence
		}
	}
}


Type And::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			if (!bool(l_t.intv)) return _FALSE;
		}
		break;

	case DOUBLE:
		{
			if (!bool(l_t.doublev)) return _FALSE;
		}
		break;

	case STRING:
		{
			if (!bool(strlen(l_t.strv))) return _FALSE;
		}
		break;

	case ARRAY:
		{
			if (!bool(l_t.vectv.size)) return _FALSE;

			bool and = false;

			for (unsigned int i = 0; i < l_t.vectv.size; ++i)
			{
				if (bool(l_t.vectv.v[i]))
				{
					and = true;
					break;
				}
			}

			if (!and) return _FALSE;
		}
		break;

	default:
		{
			bool and = false;

			for (unsigned int i = 0; i < l_t.vectv.size; ++i)
			{
				if (bool(l_t.vectv.v[i]))
				{
					and = true;
					break;
				}
			}

			if (!and) return _FALSE;
		}
		break;
	}

	switch (r_t.my_type)
	{
	case INT:
		{
			if (!bool(r_t.intv)) return _FALSE;
		}
		break;

	case DOUBLE:
		{
			if (!bool(r_t.doublev)) return _FALSE;
		}
		break;

	case STRING:
		{
			if (!bool(strlen(r_t.strv))) return _FALSE;
		}
		break;

	case ARRAY:
		{
			if (!bool(r_t.vectv.size)) return _FALSE;

			for (unsigned int i = 0; i < r_t.vectv.size; ++i)
			{
				if (bool(r_t.vectv.v[i])) return _TRUE;
			}

			return _FALSE;
		}
		break;

	default:
		{
			for (unsigned int i = 0; i < r_t.vectv.size; ++i)
			{
				if (bool(r_t.vectv.v[i])) return _TRUE;
			}

			return _FALSE;
		}
		break;
	}

	return _TRUE;
}


Type Or::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			if (bool(l_t.intv)) return _TRUE;
		}
		break;

	case DOUBLE:
		{
			if (bool(l_t.doublev)) return _TRUE;
		}
		break;

	case STRING:
		{
			if (bool(strlen(l_t.strv))) return _TRUE;
		}
		break;

	case ARRAY:
		{
			if (bool(l_t.vectv.size))
			{
				for (unsigned int i = 0; i < l_t.vectv.size; ++i)
				{
					if (bool(l_t.vectv.v[i])) return _TRUE;
				}
			}
		}
		break;

	default:
		{
			for (unsigned int i = 0; i < l_t.vectv.size; ++i)
			{
				if (bool(l_t.vectv.v[i])) return _TRUE;
			}
		}
		break;
	}

	switch (r_t.my_type)
	{
	case INT:
		{
			if (bool(r_t.intv)) return _TRUE;
		}
		break;

	case DOUBLE:
		{
			if (bool(r_t.doublev)) return _TRUE;
		}
		break;

	case STRING:
		{
			if (bool(strlen(r_t.strv))) return _TRUE;
		}
		break;

	case ARRAY:
		{
			if (bool(r_t.vectv.size))
			{
				for (unsigned int i = 0; i < r_t.vectv.size; ++i)
				{
					if (bool(r_t.vectv.v[i])) return _TRUE;
				}
			}
		}
		break;

	default:
		{
			for (unsigned int i = 0; i < r_t.vectv.size; ++i)
			{
				if (bool(r_t.vectv.v[i])) return _TRUE;
			}
		}
		break;
	}

	return _FALSE;
}


Type Eq::execute()
{
	return left->execute() == right->execute();//use overloaded operator
}


Type Neq::execute()
{
	if (left->execute() == right->execute()) return _FALSE;//use overloaded operator

	int tru = 1;
	return _TRUE;
}


Type Lt::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.intv < r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.intv < r_t.doublev)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;//string, struct can't be equal to a number
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.doublev < r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.doublev < r_t.doublev)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;
				}
			}
		}

	case STRING:
		{
			switch (r_t.my_type)
			{
			case STRING:
				{
					if (strcmp(l_t.strv, r_t.strv) < 0)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;
				}
			}
		}

	default:
		{
			return _TRUE;//arrays and structures can't be greater of each other or something
		}
	}

	return _FALSE;
}


Type Gt::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.intv > r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.intv > r_t.doublev)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;//string, struct can't be equal to a number
				}
			}
		}

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.doublev > r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.doublev > r_t.doublev)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;
				}
			}
		}

	case STRING:
		{
			switch (r_t.my_type)
			{
			case STRING:
				{
					if (strcmp(l_t.strv, r_t.strv) > 0)
						return _TRUE;
					else
						return _FALSE;
				}

			default:
				{
					return _TRUE;
				}
			}
		}

	default:
		{
			return _TRUE;
		}
	}

	return _FALSE;
}


Type Lt_or_eq::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.intv <= r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.intv <= r_t.doublev)
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
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.doublev <= r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.doublev <= r_t.doublev)
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
			switch (r_t.my_type)
			{
			case STRING:
				{
					if (strcmp(l_t.strv, r_t.strv) <= 0)
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

	default:
		{
			return _FALSE;
		}
	}

	return _FALSE;
}


Type Gt_or_eq::execute()
{
	int tru = 1;

	Type l_t = left->execute();
	Type r_t = right->execute();

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.intv >= r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.intv >= r_t.doublev)
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
			switch (r_t.my_type)
			{
			case INT:
				{
					if (l_t.doublev >= r_t.intv)
						return _TRUE;
					else
						return _FALSE;
				}

			case DOUBLE:
				{
					if (l_t.doublev >= r_t.doublev)
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
			switch (r_t.my_type)
			{
			case STRING:
				{
					if (strcmp(l_t.strv, r_t.strv) >= 0)
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

	default:
		{
			return _FALSE;
		}
	}

	return _FALSE;
}


Type Intersect::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	int intersection = 0;

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					intersection = l_t.intv & r_t.intv;

					return Type (&intersection, INT);
				}
				break;

			case DOUBLE:
				{
					intersection = l_t.intv & int(r_t.doublev);

					return Type (&intersection, INT);
				}
				break;

			default:
				{
					return _ZERO;
				}
			}
		}
		break;

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					intersection = int(l_t.doublev) & r_t.intv;

					return Type (&intersection, INT);
				}
				break;

			case DOUBLE:
				{
					intersection = int(l_t.doublev) & int(r_t.doublev);

					return Type (&intersection, INT);
				}
				break;

			default:
				{
					return _ZERO;
				}
			}
		}
		break;

	default:
		{
			return _ZERO;
		}
	}
}


Type Merge::execute()
{
	Type l_t = left->execute();
	Type r_t = right->execute();

	int merge = 0;

	switch (l_t.my_type)
	{
	case INT:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					merge = l_t.intv | r_t.intv;

					return Type (&merge, INT);
				}
				break;

			case DOUBLE:
				{
					merge = l_t.intv | int(r_t.doublev);

					return Type (&merge, INT);
				}
				break;

			default:
				{
					return _ZERO;
				}
			}
		}
		break;

	case DOUBLE:
		{
			switch (r_t.my_type)
			{
			case INT:
				{
					merge = int(l_t.doublev) | r_t.intv;

					return Type (&merge, INT);
				}
				break;

			case DOUBLE:
				{
					merge = int(l_t.doublev) | int(r_t.doublev);

					return Type (&merge, INT);
				}
				break;

			default:
				{
					return _ZERO;
				}
			}
		}
		break;

	default:
		{
			return _ZERO;
		}
	}
}


Type Assignment::execute()
{
	return left->instance() = right->execute();//Use overloaded operator
}


Type Neg::execute()
{
	Type a_t = arg->execute();

	switch (a_t.my_type)
	{
	case INT:
		{
			int neg = -a_t.intv;
			return Type(&neg, INT);
		}

	case DOUBLE:
		{
			double neg = -a_t.doublev;
			return Type(&neg, DOUBLE);
		}
		break;

	default:
		{
			return _ZERO;
		}
	}
}

Type Not::execute()
{
	int tru = 1;

	Type a_t = arg->execute();

	switch (a_t.my_type)
	{
	case INT:
		{
			if (a_t.intv)
				return _FALSE;
			else
				return _TRUE;
		}

	case DOUBLE:
		{
			if (a_t.doublev)
				return _FALSE;
			else
				return _TRUE;
		}

	case STRING:
		{
			if (strlen(a_t.strv))
				return _FALSE;
			else
				return _TRUE;
		}

	case ARRAY:
		{
			if (!bool(a_t.vectv.size)) return _TRUE;

			for (unsigned int i = 0; i < a_t.vectv.size; ++i)
			{
				if (bool(a_t.vectv.v[i])) return _FALSE;
			}

			return _TRUE;
		}

	default:
		{
			for (unsigned int i = 0; i < a_t.vectv.size; ++i)
			{
				if (bool(a_t.vectv.v[i])) return _FALSE;
			}

			return _TRUE;
		}
	}

	return _FALSE;
}

Type Inv::execute()
{
	Type a_t = arg->execute();

	switch (a_t.my_type)
	{
	case INT:
		{
			int inv = ~a_t.intv;
			return Type (&inv, INT);
		}

	case DOUBLE:
		{
			int inv = ~int(a_t.doublev);
			return Type (&inv, INT);
		}
		break;

	default:
		{
			return _ZERO;
		}
	}
}


Type Inc::execute()
{
	Type& a_t = arg->instance();

	switch (a_t.my_type)
	{
	case INT:
		{
			int tmp = a_t.intv;
			++a_t.intv;
			return Type (&tmp, INT);
		}

	case DOUBLE:
		{
			double tmp = a_t.doublev;
			++a_t.doublev;
			return Type (&tmp, DOUBLE);
		}

	default:
		{
			return _ZERO;
		}
	}
}

Type Dec::execute()
{
	Type& a_t = arg->instance();

	switch (a_t.my_type)
	{
	case INT:
		{
			int tmp = a_t.intv;
			--a_t.intv;
			return Type (&tmp, INT);
		}

	case DOUBLE:
		{
			double tmp = a_t.doublev;
			--a_t.doublev;
			return Type (&tmp, DOUBLE);
		}

	default:
		{
			return _ZERO;
		}
	}
}


Type Of::execute()
{
	Type s = str->execute();

	if (s.my_type == my_type)
	{
		return s.vectv.v[offset];
	}

	return _ZERO;
}

Type& Of::instance()
{
	Type& s = str->instance();

	if (s.my_type == my_type)
	{
		return s.vectv.v[offset];
	}

	return s;//this can cause a critical problems!!!
}


Type In::execute()
{
	Type o = left->execute();
	Type a = right->execute();

	if (a.my_type == ARRAY)
	{
		switch (o.my_type)
		{
		case INT:
			{
				return a.vectv.v[o.intv];// - 1?
			}

		case DOUBLE:
			{
				return a.vectv.v[int(o.doublev)];// - 1?
			}
		}
	}

	return _ZERO;
}

Type& In::instance()
{
	Type  o = left->execute();
	Type& a = right->instance();

	if (a.my_type == ARRAY)
	{
		switch (o.my_type)
		{
		case INT:
			{
				return a.vectv.v[o.intv];// - 1?
			}

		case DOUBLE:
			{
				return a.vectv.v[int(o.doublev)];// - 1?
			}
		}
	}

	return a;//this can cause a critical problems!!!
}


Combined::Combined ( Scope *s ) : _s (s) 
{
	operators = new std::vector<Operator*> (0);
}

Combined::~Combined()
{
	utility::killer kill;

	std::for_each (operators->begin(), operators->end(), kill);

	if (operators)	delete operators;
	if (_s)			delete _s;
}

void Combined::add ( Operator *o )
{
	operators->push_back (o);
}

Type Combined::execute()
{
	utility::executer execute;

	_s->Prepare();

	try
	{
		std::for_each (operators->begin(), operators->end(), execute);
	}
	catch (...)
	{
		_s->Clear();

		throw;
	}

	_s->Clear();

	return _ZERO;
}


Initializer::Initializer ()
{
	elements = new std::vector<Operator*> (0);
}

Initializer::~Initializer()
{
	utility::killer kill;

	std::for_each (elements->begin(), elements->end(), kill);

	if (elements) delete elements;
}

void Initializer::add ( Operator *e )
{
	elements->push_back (e);
}

Type Initializer::execute()
{
	utility::builder build;

	build = std::for_each (elements->begin(), elements->end(), build);

	return Type (build.buff.begin(), ARRAY, build.buff.size());
}


If::If ( Scope *s ) : _s (s), pred (0), top (0), bottom (0), has_bottom (0) {}

If::~If()
{
	if ( has_bottom )
	{
		delete bottom;
	}

	if (top)	delete top;
	if (pred)	delete pred;
	if (_s)		delete _s;
}

void If::add_pred ( Operator *p )
{
	pred = p;
}

void If::add_top ( Operator *t )
{
	top = t;
}

void If::add_bottom ( Operator *b )
{
	bottom = b;
	has_bottom = true;
}

Type If::execute()
{
	_s->Prepare();

	if ( bool(pred->execute()) )
	{
		try
		{
			top->execute();
		}
		catch (...)
		{
			_s->Clear();

			throw;
		}
	}
	else if ( has_bottom )
	{
		try
		{
			bottom->execute();
		}
		catch (...)
		{
			_s->Clear();

			throw;
		}
	}

	_s->Clear();

	return _ZERO;
}



While::While ( Scope *s ) : _s (s), pred (0), body (0), has_body (0) {}

While::~While()
{
	if ( has_body )
	{
		delete body;
	}

	if (pred)	delete pred;
	if (_s)		delete _s;
}

void While::add_pred ( Operator *p )
{
	pred = p;
}

void While::add_body ( Operator *b )
{
	body = b;
	has_body = true;
}

Type While::execute()
{
	_s->Prepare();

	while ( bool(pred->execute()) )
	{
		if ( has_body )
		{
			try
			{
				body->execute();
			}
			catch ( continue_level *c )
			{
				if ( --c->level > 0 )
				{
					throw;
				}
				else
				{
					delete c;//Terminate this exception, and go to next iteration
				}
			}
			catch ( break_level *b )
			{
				if ( --b->level > 0 )
				{
					throw;
				}
				else
				{
					delete b;
					_s->Clear();

					return _ZERO;//Exit cycle
				}
			}
			catch (...)
			{
				_s->Clear();

				throw;
			}
		}
	}

	_s->Clear();

	return _ZERO;
}


For::For ( Scope *s ) : _s (s), init (0), check (0), inc (0), body (0), blocks (0) {}

For::~For()
{
	if ( blocks & INIT )
	{
		delete init;
	}

	if ( blocks & CHECK )
	{
		delete check;
	}

	if ( blocks & INC )
	{
		delete inc;
	}

	if ( blocks & BODY )
	{
		delete body;
	}

	if (_s)		delete _s;
}

void For::add_init ( Operator *i )
{
	init = i;
	blocks |= INIT;
}

void For::add_check ( Operator *c )
{
	check = c;
	blocks |= CHECK;
}

void For::add_inc ( Operator *i )
{
	inc = i;
	blocks |= INC;
}

void For::add_body ( Operator *b )
{
	body = b;
	blocks |= BODY;
}

Type For::execute()
{
	_s->Prepare();

	//Now let's use bad programming style:

	if ( blocks & INIT )
	{
		init->execute();
	}

	CHECKING:

	if ( blocks & CHECK )
	{
		if ( !bool(check->execute()) )
		{
			return _ZERO;
		}
	}

	if ( blocks & BODY )
	{
		try
		{
			body->execute();
		}
		catch ( continue_level *c )
		{
			if ( --c->level > 0 )
			{
				throw;
			}
			else
			{
				delete c;
			}
		}
		catch ( break_level *b )
		{
			if ( --b->level > 0 )
			{
				throw;
			}
			else
			{
				delete b;
				_s->Clear();

				return _ZERO;
			}
		}
		catch (...)
		{
			_s->Clear();

			throw;
		}
	}

	if ( blocks & INC )
	{
		inc->execute();
	}

	goto CHECKING;

	_s->Clear();

	return _ZERO;
}

#pragma warning(default:4800)