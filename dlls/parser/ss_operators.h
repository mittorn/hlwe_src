/*===============================================================================================

		Here defined basic operators. Variables and constants defined as operators too, but all
	they do is returning their values. But this definition allows to build operators tree
	in leafs of what we will have variables and constants. They will return their values,
	then standard operators will do something with this values and then return result of
	operation to superior operator in tree etc.
		For more optimization, parser will try to calculate expressions, that containing only
	constants, during compilation.

===============================================================================================*/

#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include "ss_types.h"
#include "ss_scope.h"
#include <vector>

class Operator
{
public:

	virtual Type execute() = 0;

	virtual bool constant()
	{
		return false;//hack : can we pre-calculate some expression?
	};

	virtual ~Operator() {};
};

//======================= OBJECT-FUNCIONS. NEEDED FOR ALGORITHMS ===============================

namespace utility
{
	struct executer
	{
		void operator() (Operator *o)
		{
			o->execute();
		};
	};

	struct killer
	{
		void operator() (Operator *o)
		{
			delete o;
		};
	};

	struct builder
	{
		std::vector<Type> buff;

		builder() : buff(0) {};

		void operator() (Operator *o)
		{
			buff.push_back (o->execute());
		};
	};
};

//=========================== CONSTANT - THE SIMPLEST OPERATOR =================================

class Constant : public Operator
{
	Type value;

public:

	Constant ( Type v ) : value(v) {};

	Type execute() { return value; };

	bool constant() { return true; };
};

//============================== L-VALUE (must have an instance...) =============================
//============ It is not only for variables, but for field access operators etc =================

class LValue : public Operator
{
public:

	virtual Type& instance() = 0;//that's simple...
};

//================================= VARIABLE - SIMPLEST L-VALUE =================================

class Variable : public LValue
{
	Scope			*_s;
	unsigned int	_id;

public:

	Variable ( Scope *s, char *name );

	Type execute();
	Type& instance();
};

//========================== SIMPLE BINARY OPERATORS : +, -, etc ================================
//=======This operators can be overriden by Constants, if their arguments are constants==========

class Binary : public Operator
{
protected:

	Operator *left, *right;

public:

	Binary ( Operator *l, Operator *r ) : left (l), right (r) {};

	~Binary()
	{
		if (left) delete left;
		if (right)delete right;
	};

	bool constant()
	{
		return left->constant() && right->constant();//for ones, who wasn't learned to count
	};
};

class Summ : public Binary
{
public:

	Summ ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Diff : public Binary
{
public:

	Diff ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Mult : public Binary
{
public:

	Mult ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Div : public Binary
{
public:

	Div ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Mod : public Binary
{
public:

	Mod ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};


//====================================== LOGIC OPERATORS ========================================

class And : public Binary
{
public:

	And ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Or : public Binary
{
public:

	Or ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

//==================================== COMPARISON OPERATORS =====================================

class Eq : public Binary
{
public:

	Eq ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Neq : public Binary
{
public:

	Neq ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Lt : public Binary
{
public:

	Lt ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Gt : public Binary
{
public:

	Gt ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Lt_or_eq : public Binary
{
public:

	Lt_or_eq ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Gt_or_eq : public Binary
{
public:

	Gt_or_eq ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

//====================================== BIT SET OPERATIONS =====================================

class Intersect : public Binary
{
public:

	Intersect ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

class Merge : public Binary
{
public:

	Merge ( Operator *l, Operator *r ) : Binary (l, r) {};

	Type execute();
};

//==================================== ASSIGNMENT OPERATORS =====================================
//============================= requires l-value as a left operand ==============================

class Assignment : public Operator
{
	LValue	 *left;
	Operator *right;

public:

	Assignment ( LValue *l, Operator *r ) : left (l), right(r) {};

	~Assignment()
	{
		if (left) delete left;
		if (right)delete right;
	};

	Type execute();
};

//======================================= UNARY OPERATORS =======================================

class UnaryR : public Operator
{
protected:

	Operator *arg;

public:

	UnaryR ( Operator *a ) : arg (a) {};

	~UnaryR ()
	{
		if (arg) delete arg;
	};

	bool constant()
	{
		return arg->constant();
	};
};

class Neg : public UnaryR
{
public:

	Neg ( Operator *a ) : UnaryR (a) {};

	Type execute();
};

class Not : public UnaryR
{
public:

	Not ( Operator *a ) : UnaryR (a) {};

	Type execute();
};

class Inv : public UnaryR
{
public:

	Inv ( Operator *a ) : UnaryR (a) {};

	Type execute();
};

//============================ UNARY OPERATORS WITH LVALUE OPERAND ==============================

class UnaryL : public Operator
{
protected:

	LValue *arg;

public:

	UnaryL ( LValue *a ) : arg (a) {};

	~UnaryL ()
	{
		if (arg) delete arg;
	};
};

class Inc : public UnaryL
{
public:

	Inc ( LValue *a ) : UnaryL (a) {};

	Type execute();
};

class Dec : public UnaryL
{
public:

	Dec ( LValue *a ) : UnaryL (a) {};

	Type execute();
};


//====================================== ACCESS OPERATORS =======================================
//=================================== they all are l-values =====================================

class Of : public LValue
{
	short int		my_type;
	unsigned int	offset;
	LValue			*str;

public:

	Of ( short int t, unsigned int off, LValue *s ) : my_type (t), offset (off), str (s) {};

	~Of ()
	{
		if (str) delete str;
	};

	Type execute();
	Type& instance();

	bool constant()
	{
		return str->constant();
	};
};

class In : public LValue
{
	Operator	*left;
	LValue		*right;

public:

	In ( Operator *l, LValue *r ) : left (l),  right (r) {};

	~In ()
	{
		if (left) delete left;
		if (right)delete right;
	};

	Type execute();
	Type& instance();

	bool constant()
	{
		return left->constant() && right->constant();
	};
};

//=============================== CYCLE-INTERRUPTING OPERATORS: =================================
//============================== such as break, return or continue ==============================

class Return : public Operator
{
	Operator *value;

public:

	Return ( Operator *v ) : value (v) {};

	~Return()
	{
		if (value) delete value;
	};

	Type execute()
	{
		Type retval = value->execute();
		return Type (&retval, _RETURN);
		//This is "_return", so value will be copied-out
	};
};

class Break : public Operator
{
	int level;

public:

	Break ( int l ) : level (l) {};

	Type execute()
	{
		return Type (&level, _BREAK);
	};
};

class Continue : public Operator
{
	int level;

public:

	Continue ( int l ) : level (l) {};

	Type execute()
	{
		return Type (&level, _CONTINUE);
	};
};

//==================== COMBINED OPERATOR (or how does it being realy called?) ===================

class Combined : public Operator
{
	Scope					*_s;//This operator takes care about it's scope

	std::vector<Operator*>	*operators;

public:

	Combined ( Scope *s );
	~Combined();

	void add ( Operator *o );

	Type execute();
};

//=================================== ARRAY INITIALIZER =========================================
//===================== it is very comfortable thing for method SAVE ============================

class Initializer : public Operator
{
	std::vector<Operator*>	*elements;

public:

	Initializer ();
	~Initializer();

	void add ( Operator *o );

	Type execute();
};

//===================================== WHILE, FOR, IF ==========================================
//============================== it's very... complicated code ==================================

class If : public Operator
{
	Scope		*_s;

	Operator	*pred;
	Operator	*top, *bottom;

	bool has_bottom;

public:

	If ( Scope *s );
	~If();

	void add_pred ( Operator *p );
	void add_top ( Operator *t );
	void add_bottom ( Operator *b );

	Type execute();
};

class While : public Operator
{
	Scope		*_s;

	Operator	*pred;
	Operator	*body;

	bool has_body;

public:

	While ( Scope *s );
	~While();

	void add_pred ( Operator *p );
	void add_body ( Operator *b );

	Type execute();
};

class For : public Operator
{
	Scope		*_s;

	Operator	*init, *check, *inc;
	Operator	*body;

	char blocks;

	enum
	{
		INIT	= 1,
		CHECK	= 2,
		INC		= 4,
		BODY	= 8
	};

public:

	For ( Scope *s );
	~For();

	void add_init ( Operator *i );
	void add_check ( Operator *c );
	void add_inc ( Operator *i );
	void add_body ( Operator *b );

	Type execute();
};

#endif
