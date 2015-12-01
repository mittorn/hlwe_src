/*===============================================================================================
						Here defined all those basic elements we using
===============================================================================================*/

#ifndef _KERNEL_H_
#define _KERNEL_H_

#define MESSAGING_ROUTINE	"sscript_msgsend"

void CreateKernel ( Functions *f );

//=============================================
//	Simplifying all shit
//=============================================
class KernelFunction : public GenericFunction
{
public:

	KernelFunction (int args) : GenericFunction(args, SPECIFICATION_DEFAULT) {};

	Scope* MyScope () { return 0; };

	void AddBody ( Operator *b ) {};

	bool Forward () { return false; };

	~KernelFunction () {};
};


//============================================
//	Structure types' constructors
//============================================
class Constructor : public KernelFunction
{
	short int			type;
	std::vector<Type>	fields;

public:

	Constructor ( short int t, int a ) : KernelFunction(a), type (t), fields (a) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		fields[argi] = argv;
	};

	Type Execute ()
	{
		return Type (fields.begin(), type, GenericFunction::argc);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute () {};
};

#endif