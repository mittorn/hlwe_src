/*===============================================================================================

	Kernel of Samogon Script - all those functions, that linking Weapon Edition engine and
	script file.

===============================================================================================*/

#include "ss_functions.h"
#include "ss_kernel.h"
#include "ss_mgr.h"

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "player.h"
#include "math.h"
#include "string.h"

extern void respawn(entvars_t *pev, BOOL fCopyCorpse);
extern float SetController( void *pmodel, entvars_t *pev, int iController, float flValue );

extern int gmsgScoreInfo;

#define SS_ENTVARS_SIZE		34

Type entv[SS_ENTVARS_SIZE];
Type vect[3];

char buf[1024];

extern StringBuf pool;

#pragma warning(disable:4800)

//============================================
//	Array type constructor
//============================================
class Array : public KernelFunction
{
	int length;

public:

	Array () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				{
					length = argv.intv;
				}
				break;

			case DOUBLE:
				{
					length = int(argv.doublev);
				}
				break;
			}
		}
	};

	Type Execute ()
	{
		Type *data = new Type[length];//this can be critical in case of "length" <= 0

		Type tmp (data, ARRAY, length);

		delete [] data;

		return tmp;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		length = 0;
	};
};


//============================================
//	Basic mathematical functions of kernel
//============================================
class Cos : public KernelFunction
{
	double arg;

public:

	Cos () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		arg = cos(arg);

		return Type (&arg, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};

class Sin : public KernelFunction
{
	double arg;

public:

	Sin () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		arg = sin(arg);

		return Type (&arg, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Asin : public KernelFunction
{
	double arg;

public:

	Asin () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		arg = asin(arg);

		return Type (&arg, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Acos : public KernelFunction
{
	double arg;

public:

	Acos () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		arg = acos(arg);

		return Type (&arg, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Atan : public KernelFunction
{
	double arg;

public:

	Atan () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		arg = atan(arg);

		return Type (&arg, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Log : public KernelFunction
{
	double base;
	double arg;

public:

	Log () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				base = argv.intv;
				break;

			case DOUBLE:
				base = argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		double res = log10(arg)/log10(base);

		return Type (&res, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
		base= 0;
	};
};


class Pow : public KernelFunction
{
	double base;
	double arg;

public:

	Pow () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				base = argv.intv;
				break;

			case DOUBLE:
				base = argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		double res = pow(base, arg);

		return Type (&res, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
		base= 0;
	};
};


class Ceil : public KernelFunction
{
	double arg;

public:

	Ceil () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		int res = ceil(arg);

		return Type (&res, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Floor : public KernelFunction
{
	double arg;

public:

	Floor () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				arg = argv.intv;
				break;

			case DOUBLE:
				arg = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		int res = floor(arg);

		return Type (&res, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		arg = 0;
	};
};


class Strlen : public KernelFunction
{
	unsigned int length;

public:

	Strlen () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				length = strlen(argv.strv);
				break;
			}
		}
	};

	Type Execute ()
	{
		return Type (&length, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		length = 0;
	};
};


class Substr : public KernelFunction
{
	unsigned short int begin, end;

public:

	Substr () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				begin = min(argv.intv, 1023);
				break;

			case DOUBLE:
				begin = min((unsigned short int)argv.doublev, 1023);
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				end = min(argv.intv + 1, 1023);
				break;

			case DOUBLE:
				end = min((unsigned short int)argv.doublev + 1, 1023);
				break;
			}
		}
	};

	Type Execute ()
	{
		buf[end] = '\0';

		return Type(&buf[begin], STRING);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		begin = end = 0;
		memset(buf, 0, sizeof buf);
	};
};


class Insert : public KernelFunction
{
	unsigned short int begin;

	Type str;//((

public:

	Insert () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				str = argv;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				begin = argv.intv;
				break;

			case DOUBLE:
				begin = (unsigned short int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (str)
		{
			unsigned int shift = strlen(str.strv);
			unsigned int size = strlen(buf);

			for (unsigned int i = size; i >= begin; --i)
				if (i + shift < 1023)
					buf[i + shift] = buf[i];

			strncpy(&buf[begin], str.strv, shift);

			return Type(buf, STRING);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		begin = 0;
		memset(buf, 0, sizeof buf);
		str = _ZERO;
	};
};


class Delete : public KernelFunction
{
	unsigned short int begin, end;

public:

	Delete () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				begin = min(argv.intv, 1023);
				break;

			case DOUBLE:
				begin = min((unsigned short int)argv.doublev, 1023);
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				end = min(argv.intv, 1023);
				break;

			case DOUBLE:
				end = min((unsigned short int)argv.doublev, 1023);
				break;
			}
		}
	};

	Type Execute ()
	{
		unsigned int length = strlen(buf);

		if (begin < 0) begin = 0;
		if (end > length) end = length;

		for (unsigned int i = 0; i <= length - end; ++i)
		{
			buf[begin + i] = buf[end + i + 1];
		}

		return Type(buf, STRING);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		begin = end = 0;
		memset(buf, 0, sizeof buf);
	};
};


class Arraysize : public KernelFunction
{
	unsigned int length;

public:

	Arraysize () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case ARRAY:
				length = argv.vectv.size;
				break;
			}
		}
	};

	Type Execute ()
	{
		return Type (&length, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		length = 0;
	};
};


class Random : public KernelFunction
{
	double low;
	double high;

public:

	Random () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				low = argv.intv;
				break;

			case DOUBLE:
				low = argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				high = argv.intv;
				break;

			case DOUBLE:
				high = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		double ran = RANDOM_FLOAT(low, high);

		return Type (&ran, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		low = high = 0;
	};
};


class Numtostr : public KernelFunction
{
	union
	{
		int iv;
		float dv;//(((
	};

	char type;

public:

	Numtostr () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				type = 'i';
				iv = argv.intv;
				break;

			case DOUBLE:
				type = 'd';
				dv = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		switch (type)
		{
		case 'i':
			sprintf(buf, "%i", iv);
			break;

		case 'd':
			sprintf(buf, "%f", dv);
			break;
		}

		return Type(&buf[0], STRING);
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
		iv = 0;
		type = 'i';
		memset(buf, 0, sizeof buf);
	};
};


//============================================
//	Basic i/o functions of kernel
//============================================
class Write : public KernelFunction
{
	int entindex;
	Type message_delay;

public:

	Write () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				entindex = argv.intv;
				break;

			case DOUBLE:
				entindex = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			message_delay = argv;
		}
	};

	Type Execute ()
	{
		switch(message_delay.my_type)
		{
		case STRING:
			if (strlen(message_delay.strv) < 1024)
				strcpy(buf, message_delay.strv);
			else
			{
				strncpy(buf, message_delay.strv, 1023);
				buf[1023] = '\0';
			}
			break;

		case _INT:
			sprintf(buf, "%i", message_delay.intv);
			break;

		case DOUBLE:
			{
				float f = message_delay.doublev;
				sprintf(buf, "%f", f);
			}
			break;

		default:
			return _ZERO;
		}

		if (entindex == -2)
		{
			ALERT(at_console, buf);
			return _ZERO;
		}

		float time = strlen(buf);

		hudtextparms_t params =
		{
			-1, 0.4, //almost center of screen
			2,		//write-out text
			255, 170, 0, 255,//hlwe-style yellow
			100, 100, 100, 255,//smooth yellow
			0.01, 0.1, sqrt(time)/4, 0.2,//times...
			65535//special reserved channel
		};

		if (entindex == -1)
		{
			UTIL_HudMessageAll(params, buf);

			return _ZERO;
		}

		CBaseEntity *pPlayer = CBaseEntity::Instance(INDEXENT(entindex));

		if (!pPlayer || !pPlayer->IsPlayer())
			return _ZERO;

		UTIL_HudMessage(pPlayer, params, buf);

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		entindex = 0;
		memset (buf, 0, sizeof buf);
	};
};


//====================================
//Link samogon scripts with
//Weapon Edition engine
//====================================
class Kill : public KernelFunction
{
	int entindex, attacker, gib;

public:

	Kill () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				entindex = argv.intv;
				break;

			case DOUBLE:
				entindex = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				attacker = argv.intv;
				break;

			case DOUBLE:
				attacker = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				gib = argv.intv;
				break;

			case DOUBLE:
				gib = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (entindex <= 0 || attacker <= 0) return _ZERO;

		edict_t *edict = INDEXENT(entindex);
		edict_t *attack = INDEXENT(attacker);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);
			CBaseEntity *pAttacker = CBaseEntity::Instance(attack);

			if (pEnt->IsPlayer()) return _ZERO;

			if (pEnt) pEnt->Killed((pAttacker?(pAttacker->pev):NULL), gib);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		entindex = attacker = gib = 0;
	};
};


class Damage : public KernelFunction
{
	int entindex, attacker, type;
	float value;

public:

	Damage () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				entindex = argv.intv;
				break;

			case DOUBLE:
				entindex = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				attacker = argv.intv;
				break;

			case DOUBLE:
				attacker = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = argv.intv;
				break;

			case DOUBLE:
				value = argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				type = argv.intv;
				break;

			case DOUBLE:
				type = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(entindex);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt) pEnt->TakeDamage(VARS(INDEXENT(attacker)), VARS(INDEXENT(attacker)), value, type);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		entindex = attacker = value = type = 0;
	};
};


class Trigger : public KernelFunction
{
	int entindex, user, type;
	float value;

public:

	Trigger () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				entindex = argv.intv;
				break;

			case DOUBLE:
				entindex = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				user = argv.intv;
				break;

			case DOUBLE:
				user = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				type = argv.intv;
				break;

			case DOUBLE:
				type = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = argv.intv;
				break;

			case DOUBLE:
				value = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(entindex);
		CBaseEntity *other = CBaseEntity::Instance(INDEXENT(user));

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt) pEnt->Use(other, other, (USE_TYPE)type, value);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		entindex = user = value = type = 0;
	};
};


class Traceline : public KernelFunction
{
	int igmon, igglass, ignore;
	Vector start, end;

public:

	Traceline () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						start.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						start.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						start.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						start.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						start.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						start.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						end.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						end.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						end.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						end.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						end.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						end.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				igmon = argv.intv;
				break;

			case DOUBLE:
				igmon = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				igglass = argv.intv;
				break;

			case DOUBLE:
				igglass = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				ignore = argv.intv;
				break;

			case DOUBLE:
				ignore = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		TraceResult tr;
		UTIL_TraceLine (start, end, (IGNORE_MONSTERS)igmon, (IGNORE_GLASS)igglass, (ignore?(INDEXENT(ignore)):(NULL)), &tr);

		Type tmp[10];
		double tmpdbl;

		tmp[0] = Type(&tr.fAllSolid, _INT);
		tmp[1] = Type(&tr.fStartSolid, _INT);
		tmp[2] = Type(&tr.fInOpen, _INT);
		tmp[3] = Type(&tr.fInWater, _INT);

		tmpdbl = tr.flFraction;
		tmp[4] = Type(&tmpdbl, DOUBLE);

		tmpdbl = tr.vecEndPos.x;
		vect[0] = Type(&tmpdbl, DOUBLE);
		tmpdbl = tr.vecEndPos.y;
		vect[1] = Type(&tmpdbl, DOUBLE);
		tmpdbl = tr.vecEndPos.z;
		vect[2] = Type(&tmpdbl, DOUBLE);
		tmp[5] = Type(&vect[0], VECTOR_T, 3);

		tmpdbl = tr.flPlaneDist;
		tmp[6] = Type(&tmpdbl, DOUBLE);

		tmpdbl = tr.vecPlaneNormal.x;
		vect[0] = Type(&tmpdbl, DOUBLE);
		tmpdbl = tr.vecPlaneNormal.y;
		vect[1] = Type(&tmpdbl, DOUBLE);
		tmpdbl = tr.vecPlaneNormal.z;
		vect[2] = Type(&tmpdbl, DOUBLE);
		tmp[7] = Type(&vect[0], VECTOR_T, 3);

		int index = ENTINDEX(tr.pHit);
		tmp[8] = Type(&index, _INT);

		tmp[9] = Type(&tr.iHitgroup, _INT);

		return Type(&tmp[0], TRACE_RESULT_T, 10);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		ignore = igmon = igglass = 0;
		start = end = g_vecZero;

		for (int i = 0; i < 3; ++i)
			vect[i] = _ZERO;
	};
};


class CreateScripted : public KernelFunction
{
	Vector pos, angl;
	int ent;

public:

	CreateScripted () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy (buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						pos.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						pos.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						pos.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						pos.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						pos.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						pos.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						angl.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						angl.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						angl.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						angl.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						angl.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						angl.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				ent = argv.intv;
				break;

			case DOUBLE:
				ent = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(ent);

		//Code from CBaseEntity::Create
		edict_t	*pent;
		CBaseEntity *pEntity;

		pent = CREATE_NAMED_ENTITY( MAKE_STRING("info_empty") );

		if ( FNullEnt( pent ) )
		{
			ALERT ( at_console, "CreateScripted failure...\n" );
			return _ZERO;
		}

		pEntity = CBaseEntity::Instance( pent );
		pEntity->pev->owner = edict;
		pEntity->pev->origin = pos;
		pEntity->pev->angles = angl;
		pEntity->script_file = MAKE_STRING(pool.ALLOC_MEM(buf));
		DispatchSpawn( pent );
		//End copypaste

		if (pEntity)
		{
			int index = pEntity->entindex();

			return Type(&index, _INT);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		angl = pos = g_vecZero;
		memset (buf, 0, sizeof buf);
		ent = 0;
	};
};


class Setctl : public KernelFunction
{
	int ent, num;
	double pos;

public:

	Setctl () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				ent = argv.intv;
				break;

			case DOUBLE:
				ent = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				num = argv.intv;
				break;

			case DOUBLE:
				num = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				pos = argv.intv;
				break;

			case DOUBLE:
				pos = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(ent);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt && !pEnt->IsBSPModel())
			{
				void *pmodel = GET_MODEL_PTR( edict );

				if (pmodel)
					SetController( pmodel, VARS(edict), num, pos );
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		ent = num = pos = 0;
	};
};

class Precachemodel : public KernelFunction
{
public:

	Precachemodel () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy (buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
	};

	Type Execute ()
	{
		(*g_engfuncs.pfnPrecacheModel)((char*)pool.ALLOC_MEM(buf));

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		memset (buf, 0, sizeof buf);
	};
};

class Precachesound : public KernelFunction
{
public:

	Precachesound () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy (buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
	};

	Type Execute ()
	{
		(*g_engfuncs.pfnPrecacheSound)((char*)pool.ALLOC_MEM(buf));

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		memset (buf, 0, sizeof buf);
	};
};


class Setmodel : public KernelFunction
{
	int ent;

public:

	Setmodel () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				ent = argv.intv;
				break;

			case DOUBLE:
				ent = (int)argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy (buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(ent);

		if (edict && !edict->free)
		{
			SET_MODEL(edict, buf);//(char*)STRING(ALLOC_STRING( ???
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		memset (buf, 0, sizeof buf);
		ent = 0;
	};
};


class Setsize : public KernelFunction
{
	int ent;
	Vector min;
	Vector max;

public:

	Setsize () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				ent = argv.intv;
				break;

			case DOUBLE:
				ent = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						min.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						min.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						min.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						min.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						min.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						min.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						max.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						max.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						max.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						max.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						max.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						max.z = argv.vectv.v[2].doublev;
				}
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(ent);

		if (edict && !edict->free)
		{
			UTIL_SetSize(VARS(edict), min, max);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		min = max = g_vecZero;
		ent = 0;
	};
};


class Getcenter : public KernelFunction
{
	int index;

public:

	Getcenter () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		double x,y,z;

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt)
			{
				Vector v = pEnt->Center();

				x = v.x;
				y = v.y;
				z = v.z;

				vect[0] = Type(&x, DOUBLE);
				vect[1] = Type(&y, DOUBLE);
				vect[2] = Type(&z, DOUBLE);

				return Type(&vect, VECTOR_T, 3);
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;
		
		vect[0] = vect[1] = vect[2] = _ZERO;
	};
};


class Isplayer : public KernelFunction
{
	int index;

public:

	Isplayer () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		int tru = 1;

		CBaseEntity *pEnt = CBaseEntity::Instance(INDEXENT(index));

		if (pEnt && pEnt->IsPlayer())
			return _TRUE;

		return _FALSE;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;
	};
};


class Playerbyindex : public KernelFunction
{
	int index;

public:

	Playerbyindex () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		int plr = 0;

		if ( index > 0 && index <= gpGlobals->maxClients )
		{
			edict_t *pPlayerEdict = INDEXENT( index );

			if ( pPlayerEdict && !pPlayerEdict->free )
			{
				plr = index;
			}
		}
		
		return Type(&plr, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;
	};
};


class Entitybyclassname : public KernelFunction
{
	int previous;

public:

	Entitybyclassname () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				buf[1023] = '\0';
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				previous = argv.intv;
				break;

			case DOUBLE:
				previous = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		CBaseEntity *pEnt = UTIL_FindEntityByClassname((previous>0)?(CBaseEntity::Instance(INDEXENT(previous))):(NULL), buf);//(char*)STRING(ALLOC_STRING( ???

		if (!pEnt)
			return _ZERO;

		int index = pEnt->entindex();

		return Type(&index, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		previous = 0;
		memset(buf, 0, sizeof buf);
	};
};


class Entitybytargetname : public KernelFunction
{
	int previous;

public:

	Entitybytargetname () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				buf[1023] = '\0';
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				previous = argv.intv;
				break;

			case DOUBLE:
				previous = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		CBaseEntity *pEnt = UTIL_FindEntityByTargetname((previous>0)?(CBaseEntity::Instance(INDEXENT(previous))):(NULL), buf);//(char*)STRING(ALLOC_STRING( ???

		if (!pEnt)
			return _ZERO;

		int index = pEnt->entindex();

		return Type(&index, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		previous = 0;
		memset(buf, 0, sizeof buf);
	};
};


class Entityinsphere : public KernelFunction
{
	int previous;
	double range;
	Vector center;//suxxx

public:

	Entityinsphere () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						center.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						center.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						center.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						center.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						center.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						center.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				range = argv.intv;
				break;

			case DOUBLE:
				range = argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				previous = argv.intv;
				break;

			case DOUBLE:
				previous = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		CBaseEntity *pEnt = UTIL_FindEntityInSphere((previous>0)?(CBaseEntity::Instance(INDEXENT(previous))):(NULL), center, range);

		if (!pEnt)
			return _ZERO;

		int index = pEnt->entindex();

		return Type(&index, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		previous = range = 0;
		center = g_vecZero;
	};
};


class Time : public KernelFunction
{
public:

	Time () : KernelFunction(0) {};

	void PassArg ( unsigned int argi, Type argv ){};

	Type Execute ()
	{
		double time = gpGlobals->time;

		return Type(&time, DOUBLE);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute (){};
};


class Maxplayers : public KernelFunction
{
public:

	Maxplayers () : KernelFunction(0) {};

	void PassArg ( unsigned int argi, Type argv ){};

	Type Execute ()
	{
		return Type(&gpGlobals->maxClients, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute (){};
};


class Maxents : public KernelFunction
{
public:

	Maxents () : KernelFunction(0) {};

	void PassArg ( unsigned int argi, Type argv ){};

	Type Execute ()
	{
		int count = (*g_engfuncs.pfnNumberOfEntities)();
		return Type(&count, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute (){};
};



//Conversion from samogon types to c++ types
#define _UNPACK_VECTOR(a, b)\
if (entv[(a)].my_type == VECTOR_T)\
{\
if (entv[(a)].vectv.v[0].my_type == DOUBLE)\
(b).x = entv[(a)].vectv.v[0].doublev;\
else if (entv[(a)].vectv.v[0].my_type == _INT)\
(b).x = entv[(a)].vectv.v[0].intv;\
if (entv[(a)].vectv.v[1].my_type == DOUBLE)\
(b).y = entv[(a)].vectv.v[1].doublev;\
else if (entv[(a)].vectv.v[1].my_type == _INT)\
(b).y = entv[(a)].vectv.v[1].intv;\
if (entv[(a)].vectv.v[2].my_type == DOUBLE)\
(b).z = entv[(a)].vectv.v[2].doublev;\
else if (entv[(a)].vectv.v[2].my_type == _INT)\
(b).z = entv[(a)].vectv.v[2].intv;\
}


#define _UNPACK_DIGIT(a, b)\
if (entv[(a)].my_type == _INT)\
(b) = entv[(a)].intv;\
else if (entv[(a)].my_type == DOUBLE)\
(b) = entv[(a)].doublev;


#define _UNPACK_STRING(a, b)\
if (entv[(a)].my_type == STRING)\
(b) = MAKE_STRING(pool.ALLOC_MEM(entv[(a)].strv));


#define _PACK_VECTOR(a, b)		\
tmpdbl = (b).x;\
vect[0] = Type(&tmpdbl, DOUBLE);\
tmpdbl = (b).y;\
vect[1] = Type(&tmpdbl, DOUBLE);\
tmpdbl = (b).z;\
vect[2] = Type(&tmpdbl, DOUBLE);\
entv[(a)] = Type(&vect[0], VECTOR_T, 3);


#define _PACK_DOUBLE(a, b)		\
tmpdbl = (b);\
entv[(a)] = Type(&tmpdbl, DOUBLE);
//Conversion end



class GetPev : public KernelFunction
{
	int index;

public:

	GetPev () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (index < 1) return _ZERO;

		entvars_t *pev = VARS(INDEXENT(index));

		if (!pev) return _ZERO;

		double tmpdbl;

		entv[0] = Type((char*)STRING(pev->classname), STRING);

		_PACK_VECTOR(1, pev->origin);
		_PACK_VECTOR(2, pev->velocity);
		_PACK_VECTOR(3, pev->angles);
		_PACK_VECTOR(4, pev->avelocity);
		_PACK_VECTOR(5, pev->v_angle);

		_PACK_DOUBLE(6, pev->nextthink);

		entv[7] = Type(&pev->movetype, _INT);
		entv[8] = Type(&pev->solid, _INT);
		entv[9] = Type(&pev->skin, _INT);
		entv[10] = Type(&pev->body, _INT);
		entv[11] = Type(&pev->effects, _INT);

		_PACK_DOUBLE(12, pev->gravity);
		_PACK_DOUBLE(13, pev->friction);

		entv[14] = Type(&pev->sequence, _INT);

		_PACK_DOUBLE(15, pev->frame);
		_PACK_DOUBLE(16, pev->scale);

		entv[17] = Type(&pev->rendermode, _INT);

		_PACK_DOUBLE(18, pev->renderamt);


		_PACK_VECTOR(19, pev->rendercolor);

		entv[20] = Type(&pev->renderfx, _INT);

		_PACK_DOUBLE(21, pev->health);
		_PACK_DOUBLE(22, pev->takedamage);

		entv[23] = Type(&pev->deadflag, _INT);
		entv[24] = Type(&pev->flags, _INT);

		CBaseEntity *inst = CBaseEntity::Instance(pev);

		if (inst->IsPlayer())
		{
			CBasePlayer *pplayer = (CBasePlayer*)inst;
			int team = g_pGameRules->GetTeamIndex( pplayer->m_szTeamName );

			entv[25] = Type(&team, _INT);
		}
		else
		{
			int neg = -1;
			entv[25] = Type(&neg, _INT);
		}

		_PACK_DOUBLE(26, pev->armorvalue);

		entv[27] = Type(&pev->waterlevel, _INT);

		entv[28] = Type((char*)STRING(pev->target), STRING);
		entv[29] = Type((char*)STRING(pev->targetname), STRING);

		_PACK_DOUBLE(30, pev->speed);
		_PACK_DOUBLE(31, pev->maxspeed);
		_PACK_DOUBLE(32, pev->fov);

		entv[33] = Type((char*)STRING(pev->netname), STRING);


		return Type(&entv[0], ENTVARS_T, SS_ENTVARS_SIZE);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;

		for (int i = 0; i < SS_ENTVARS_SIZE; ++i)
			entv[i] = _ZERO;
		
		vect[0] = vect[1] = vect[2] = _ZERO;
	};
};


class PutPev : public KernelFunction
{
	int index;

public:

	PutPev () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			if (argv.my_type == ENTVARS_T)
			{
				for (int i = 0; i < SS_ENTVARS_SIZE; ++i)
				{
					entv[i] = argv.vectv.v[i];
				}
			}
		}
	};

	Type Execute ()
	{
		if (index < 1) return _ZERO;

		entvars_t *pev = VARS(INDEXENT(index));

		if (!pev) return _ZERO;

		//classname is readonly field

		_UNPACK_VECTOR(1, pev->origin);
		_UNPACK_VECTOR(2, pev->velocity);
		_UNPACK_VECTOR(3, pev->angles);
		_UNPACK_VECTOR(4, pev->avelocity);

		//v_angle is readonly field

		_UNPACK_DIGIT(6, pev->nextthink);
		_UNPACK_DIGIT(7, pev->movetype);
		_UNPACK_DIGIT(8, pev->solid);
		_UNPACK_DIGIT(9, pev->skin,);
		_UNPACK_DIGIT(10, pev->body);
		_UNPACK_DIGIT(11, pev->effects);
		_UNPACK_DIGIT(12, pev->gravity);
		_UNPACK_DIGIT(13, pev->friction);
		_UNPACK_DIGIT(14, pev->sequence);
		_UNPACK_DIGIT(15, pev->frame);
		_UNPACK_DIGIT(16, pev->scale);
		_UNPACK_DIGIT(17, pev->rendermode);
		_UNPACK_DIGIT(18, pev->renderamt);

		_UNPACK_VECTOR(19, pev->rendercolor);

		_UNPACK_DIGIT(20, pev->renderfx);
		_UNPACK_DIGIT(21, pev->health);
		_UNPACK_DIGIT(22, pev->takedamage);
		_UNPACK_DIGIT(23, pev->deadflag);
		_UNPACK_DIGIT(24, pev->flags);

		int team;

		_UNPACK_DIGIT(25, team);

		CBaseEntity *inst = CBaseEntity::Instance(pev);

		if (inst->pev->team != team && inst->IsPlayer() && g_pGameRules->IsTeamplay() && g_pGameRules->IsValidTeam(g_pGameRules->GetIndexedTeamName(team)))
		{
			CBasePlayer *pplayer = (CBasePlayer*)inst;
			g_pGameRules->ChangePlayerTeam(pplayer, g_pGameRules->GetIndexedTeamName(team), true, false);
		}

		_UNPACK_DIGIT(26, pev->armorvalue);

		//Waterlevel is readonly field

		_UNPACK_STRING(28, pev->target);
		_UNPACK_STRING(29, pev->targetname);

		_UNPACK_DIGIT(30, pev->speed);
		_UNPACK_DIGIT(31, pev->maxspeed);
		_UNPACK_DIGIT(32, pev->fov);

		//Netname is readonly field

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;

		for (int i = 0; i < SS_ENTVARS_SIZE; ++i)
			entv[i] = _ZERO;
	};
};

//====================================
//	Game rules management
//====================================
class Respawn : public KernelFunction
{
	int index;

public:

	Respawn () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (index)
		{
			CBaseEntity* pEnt = CBaseEntity::Instance(INDEXENT(index));

			if (!pEnt) return _ZERO;

			if (pEnt->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

				if ( pPlayer->IsObserver() || !pPlayer->IsAlive() )
				{
					pPlayer->EndObserver();//Special OBSERVER stuff
					pPlayer->pev->nextthink = -1;//Is that necessary?
				}

				if (pPlayer->m_pCarryingObject != NULL)
					pPlayer->m_pCarryingObject->Use(pPlayer, pPlayer, USE_TOGGLE, 3);//COU_RETURN = 3

				if (pPlayer->m_iClass == 10)
					pPlayer->m_iKit = RANDOM_LONG (0,8);//special CTF stuff

				DeactivateSatchels( pPlayer );
				DeactivateBiomass( pPlayer );
				DeactivateTurrets( pPlayer );

				if (pPlayer->FlashlightIsOn())
					pPlayer->FlashlightTurnOff();

				if (pPlayer->m_fHeavyArmor)
					g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->oldmodel);

				ClearBits( pPlayer->pev->flags, FL_ONGROUND );

				if ( pPlayer->m_pSpecTank != NULL )
				{
					pPlayer->m_pSpecTank->Use( pPlayer, pPlayer, USE_OFF, 0 );
					pPlayer->m_pSpecTank = NULL;
				}

				if (pPlayer->m_fCloakActivated)
					pPlayer->CloakToggle(FALSE);

				if (pPlayer->m_fAntigravActivated)
					pPlayer->AntigravToggle(FALSE);

				pPlayer->RemoveAllItems(1);

				respawn( pPlayer->pev, false );
			}
			else pEnt->ReSpawn();
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;
	};
};


class Allowbuy : public KernelFunction
{
	bool value;

public:

	Allowbuy () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = bool(argv.intv);
				break;

			case DOUBLE:
				value = bool(argv.doublev);
				break;
			}
		}
	};

	Type Execute ()
	{
		CVAR_SET_FLOAT( "mp_nobuy", !value );

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		value = false;
	};
};


class Addmoney : public KernelFunction
{
	int index, value;

public:

	Addmoney () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = argv.intv;
				break;

			case DOUBLE:
				value = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

				int prev = pPlayer->m_flMoneyAmount;
				pPlayer->AddMoney(value);

				return Type(&prev, _INT);
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = value = 0;
	};
};


class Addfrags : public KernelFunction
{
	int index, value;

public:

	Addfrags () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = argv.intv;
				break;

			case DOUBLE:
				value = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *pKiller = INDEXENT(index);

		if (!pKiller) return _ZERO;

		entvars_t *pev = VARS(pKiller);

		if (pev)
		{
			int prev = pev->frags;
			pev->frags += value;

			CBaseEntity *ep = CBaseEntity::Instance( pKiller );

			if ( ep && value && ep->Classify() == CLASS_PLAYER )
			{
				CBasePlayer *PK = (CBasePlayer*)ep;

				MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
					WRITE_BYTE( ENTINDEX(PK->edict()) );
					WRITE_SHORT( PK->pev->frags );
					WRITE_SHORT( PK->m_iDeaths );
					WRITE_SHORT( 0 );
					WRITE_SHORT( g_pGameRules->GetTeamIndex( PK->m_szTeamName) + 1 );
				MESSAGE_END();
			}

			return Type(&prev, _INT);
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = value = 0;
	};
};


class Addwins : public KernelFunction
{
	int index, value;

public:

	Addwins () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				value = argv.intv;
				break;

			case DOUBLE:
				value = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (index < 0 || index > MAX_TEAMS) return _ZERO;

		int prev = g_pGameRules->AddScoreToTeam(index, value);
		return Type(&prev, _INT);
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = value = 0;
	};
};


class GiveWeapon : public KernelFunction
{
	int index;

public:

	GiveWeapon () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt && pEnt->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

				pPlayer->GiveNamedItem(pool.ALLOC_MEM(buf));
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = 0;
		memset(buf, 0, sizeof buf);
	};
};


class Giveammo : public KernelFunction
{
	int index, amount;

public:

	Giveammo () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				amount = argv.intv;
				break;

			case DOUBLE:
				amount = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt && pEnt->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

				pPlayer->GiveAmmo(amount, buf, 999);
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = amount = 0;
		memset(buf, 0, sizeof buf);
	};
};


class StripAll : public KernelFunction
{
	int index, suit;

public:

	StripAll () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				suit = argv.intv;
				break;

			case DOUBLE:
				suit = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt && pEnt->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

				pPlayer->RemoveAllItems(suit);
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = suit = 0;
	};
};


//====================================
// Simple game effects creation
//====================================
class Screenfade : public KernelFunction
{
	int index, alpha, flags;
	float time, hold;
	Vector RGB;

public:

	Screenfade () : KernelFunction(6) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						RGB.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						RGB.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						RGB.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						RGB.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						RGB.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						RGB.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				time = argv.intv;
				break;

			case DOUBLE:
				time = argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				hold = argv.intv;
				break;

			case DOUBLE:
				hold = argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				alpha = argv.intv;
				break;

			case DOUBLE:
				alpha = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 5)
		{
			switch (argv.my_type)
			{
			case _INT:
				flags = argv.intv;
				break;

			case DOUBLE:
				flags = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		if (index == -1)
		{
			UTIL_ScreenFadeAll(RGB, time, hold, alpha, flags);
		}
		else
		{
			edict_t *edict = INDEXENT(index);

			if (edict && !edict->free)
			{
				CBaseEntity *pEnt = CBaseEntity::Instance(edict);

				if (pEnt)
				{
					UTIL_ScreenFade(pEnt, RGB, time, hold, alpha, flags);
				}
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = time = hold = alpha = flags = 0;
		RGB = g_vecZero;
	};
};


class PlaySound : public KernelFunction
{
	int index, channel;
	float vol, att;

public:

	PlaySound () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case _INT:
				index = argv.intv;
				break;

			case DOUBLE:
				index = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				buf[1023] = '\0';
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				channel = argv.intv;
				break;

			case DOUBLE:
				channel = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				vol = argv.intv;
				break;

			case DOUBLE:
				vol = argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				att = argv.intv;
				break;

			case DOUBLE:
				att = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(index);

		if (edict && !edict->free)
		{
			EMIT_SOUND(edict, channel, buf, vol, att);//(char*)STRING(ALLOC_STRING( ???
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		index = channel = vol = att = 0;
		memset (buf, 0, sizeof buf);
	};
};


class PlayAmbSound : public KernelFunction
{
	Vector origin;
	float vol, att;

public:

	PlayAmbSound () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						origin.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						origin.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						origin.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						origin.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						origin.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						origin.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case STRING:
				strncpy(buf, argv.strv, min(strlen(argv.strv), 1023));
				buf[1023] = '\0';
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				vol = argv.intv;
				break;

			case DOUBLE:
				vol = argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				att = argv.intv;
				break;

			case DOUBLE:
				att = argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		UTIL_EmitAmbientSound ( ENT(0), origin, buf, vol, att, 0, PITCH_NORM);//(char*)STRING(ALLOC_STRING( ???

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		vol = att = 0;
		memset (buf, 0, sizeof buf);
		origin = g_vecZero;
	};
};


class Trailfx : public KernelFunction
{
	Vector origin;
	int entindex, type;

public:

	Trailfx () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						origin.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						origin.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						origin.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						origin.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						origin.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						origin.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				entindex = argv.intv;
				break;

			case DOUBLE:
				entindex = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				type = argv.intv;
				break;

			case DOUBLE:
				type = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		FX_Trail(origin, entindex, type);

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		origin = g_vecZero;
		entindex = type = 0;
	};
};


class Explfx : public KernelFunction
{
	Vector origin;
	int type;

public:

	Explfx () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						origin.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						origin.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						origin.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						origin.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						origin.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						origin.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				type = argv.intv;
				break;

			case DOUBLE:
				type = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		FX_Explosion(origin, type);

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		origin = g_vecZero;
		type = 0;
	};
};


//====================================
//	Additional functions
//====================================
class Firebullets : public KernelFunction
{
	Vector vecSrc, vecDirShooting, vecSpread;
	float flDistance;
	int iBulletType;
	int pevAttacker;

public:

	Firebullets () : KernelFunction(6) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecSrc.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecSrc.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecSrc.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecSrc.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecSrc.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecSrc.z = argv.vectv.v[2].doublev;
				}
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecDirShooting.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecDirShooting.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecDirShooting.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecDirShooting.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecDirShooting.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecDirShooting.z = argv.vectv.v[2].doublev;
				}
			}
		}
		if (argi == 2)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecSpread.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecSpread.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecSpread.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecSpread.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecSpread.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecSpread.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				flDistance = argv.intv;
				break;

			case DOUBLE:
				flDistance = argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				iBulletType = argv.intv;
				break;

			case DOUBLE:
				iBulletType = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 5)
		{
			switch (argv.my_type)
			{
			case _INT:
				pevAttacker = argv.intv;
				break;

			case DOUBLE:
				pevAttacker = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(pevAttacker);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt)
			{
				pEnt->FireMagnumBullets(vecSrc, vecDirShooting.Normalize(), vecSpread, flDistance, iBulletType, VARS(edict));
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		vecSrc = vecDirShooting = vecSpread = g_vecZero;
		iBulletType = pevAttacker = flDistance = 0;
	};
};


class Firebeam : public KernelFunction
{
	Vector vecSrc, vecDirShooting, vecSpread;
	float flDamage;
	int iBeamType;
	int pevAttacker;

public:

	Firebeam () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecSrc.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecSrc.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecSrc.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecSrc.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecSrc.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecSrc.z = argv.vectv.v[2].doublev;
				}
			}
		}
		if (argi == 1)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecDirShooting.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecDirShooting.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecDirShooting.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecDirShooting.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecDirShooting.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecDirShooting.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				flDamage = argv.intv;
				break;

			case DOUBLE:
				flDamage = argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				iBeamType = argv.intv;
				break;

			case DOUBLE:
				iBeamType = (int)argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				pevAttacker = argv.intv;
				break;

			case DOUBLE:
				pevAttacker = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		edict_t *edict = INDEXENT(pevAttacker);

		if (edict && !edict->free)
		{
			CBaseEntity *pEnt = CBaseEntity::Instance(edict);

			if (pEnt)
			{
				pEnt->FireBeam(vecSrc, vecDirShooting.Normalize(), iBeamType, flDamage, VARS(edict));
			}
		}

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		vecSrc = vecDirShooting = g_vecZero;
		iBeamType = pevAttacker = flDamage = 0;
	};
};



class BOOM : public KernelFunction
{
	Vector vecSrc;
	int Inflictor;
	float flDamage, flRadius;
	int bitsDamageType;

public:

	BOOM () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		if (argi == 0)
		{
			switch (argv.my_type)
			{
			case VECTOR_T:
				{
					if (argv.vectv.v[0].my_type == _INT)
						vecSrc.x = argv.vectv.v[0].intv;
					else if (argv.vectv.v[0].my_type == DOUBLE)
						vecSrc.x = argv.vectv.v[0].doublev;

					if (argv.vectv.v[1].my_type == _INT)
						vecSrc.y = argv.vectv.v[1].intv;
					else if (argv.vectv.v[1].my_type == DOUBLE)
						vecSrc.y = argv.vectv.v[1].doublev;

					if (argv.vectv.v[2].my_type == _INT)
						vecSrc.z = argv.vectv.v[2].intv;
					else if (argv.vectv.v[2].my_type == DOUBLE)
						vecSrc.z = argv.vectv.v[2].doublev;
				}
			}
		}
		else if (argi == 1)
		{
			switch (argv.my_type)
			{
			case _INT:
				Inflictor = argv.intv;
				break;

			case DOUBLE:
				Inflictor = (int)argv.doublev;
				break;
			}
		}
		if (argi == 2)
		{
			switch (argv.my_type)
			{
			case _INT:
				flDamage = argv.intv;
				break;

			case DOUBLE:
				flDamage = argv.doublev;
				break;
			}
		}
		else if (argi == 3)
		{
			switch (argv.my_type)
			{
			case _INT:
				flRadius = argv.intv;
				break;

			case DOUBLE:
				flRadius = argv.doublev;
				break;
			}
		}
		else if (argi == 4)
		{
			switch (argv.my_type)
			{
			case _INT:
				bitsDamageType = argv.intv;
				break;

			case DOUBLE:
				bitsDamageType = (int)argv.doublev;
				break;
			}
		}
	};

	Type Execute ()
	{
		RadiusDamage(vecSrc, VARS(INDEXENT(Inflictor)), VARS(INDEXENT(Inflictor)), flDamage, flRadius, CLASS_NONE, bitsDamageType);

		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		vecSrc = g_vecZero;
		Inflictor = bitsDamageType = flRadius = flDamage = 0;
	};
};

extern int errors;

class Invokation : public KernelFunction
{
	Type args, message;
	int entindex;

public:

	Invokation () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{
		switch (argi)
		{
		case 0:
			{
				if (argv.my_type == _INT)
					entindex = argv.intv;
				else if (argv.my_type = DOUBLE)
					entindex = (int)argv.doublev;
				else
					entindex = 0;
			}
			break;

		case 1:
			{
				message = argv;
			}
			break;

		case 2:
			{
				args = argv;
			}
			break;
		}
	};

	Type Execute ()
	{
		Type bad (NULL, BAD_INVOKATION);//"BAD_INVOKATION" constant

		if (!entindex)
			return bad;

		//Find entity by index:
		CBaseEntity *pEnt = CBaseEntity::Instance(INDEXENT(entindex));

		if (!pEnt || !pEnt->scripted)
			return bad;

		//Now lookup function in fntable of this entity:
		unsigned int id = pEnt->my_script->funcs->GetId (message.strv);

		if (id == 0)
			return bad;//There's no function in table!

		GenericFunction *pfn = pEnt->my_script->funcs->GetFn (id);

		if (pfn->argc == args.vectv.size)
		{
			if (errors < 1)
			{
				pfn->PreExecute ();

				for (int i = 0; i < pfn->argc; ++i)
				{
					pfn->PassArg (i, args.vectv.v[i]);
				}

				return pfn->Execute ();
			}
		}

		return bad;//Arguments count mismatch!
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{
		entindex = 0;
	};
};

//====================================
//So, we declaring all those basic
//elements in global scope
//====================================

void CreateKernel ( Functions *f )
{
	//Math
	f->Declare("cos", new Cos);//1 arg (double), returns double
	f->Declare("sin", new Sin);//1 arg (double), returns double
	f->Declare("arcsin", new Asin);//1 arg (double), returns double
	f->Declare("arccos", new Acos);//1 arg (double), returns double
	f->Declare("arctg", new Atan);//1 arg (double), returns double
	f->Declare("log", new Log);//2 args (double), returns double
	f->Declare("pow", new Pow);//2 args (double), returns double
	f->Declare("ceil", new Ceil);//1 arg (double), returns double
	f->Declare("floor", new Floor);//1 arg (double), returns double
	f->Declare("random", new Random);//2 args (double), returns double

	//Types
	f->Declare("Array", new Array);//1 arg(int), returns array
	f->Declare("num_to_str", new Numtostr);//1 arg (double/int), returns string
	f->Declare("strlen", new Strlen);//1 arg (string), returns int
	f->Declare("sub_str", new Substr);//string = sub_str(string, first, last)
	f->Declare("del_str", new Delete);//string = del_str(string, first, last)
	f->Declare("ins_str", new Insert);//string = ins_str(string, substring, first)
	f->Declare("arraysize", new Arraysize);//1 arg (array), returns int

	//User I/O
	f->Declare("write", new Write);//2 args (int, (string/int/double)), returns 0

	//Engine link
	f->Declare("Kill", new Kill);//Kill(victim, killer, gib?)
	f->Declare("Damage", new Damage);//Damage(victim, attacker, value, type)
	f->Declare("Use", new Trigger);//Use(object, subject, type, value)
	f->Declare("TraceLine", new Traceline);//result = TraceLine(v_start, v_end, ignore_monsters, ignore_glass, ignore_entindex);
	f->Declare("CreateScripted", new CreateScripted);//entindex2 = CreateScripted(file, pos, angl, entindex1)
	f->Declare("Center", new Getcenter);//1 arg (int), returns vector
	f->Declare("SET_CONTROLLER", new Setctl);//SET_CONTROLLER(entindex, num, val)
	f->Declare("LOAD_MODEL", new Precachemodel);//1 arg (string), returns 0
	f->Declare("SET_MODEL", new Setmodel);//2 args (int, string), returns 0
	f->Declare("SET_SIZE", new Setsize);//3 args (int, vector, vector), returns 0
	f->Declare("FindEntityInSphere", new Entityinsphere);//entindex = FindEntityInSphere(vec_center, raddius, entindex)
	f->Declare("FindEntityByName", new Entitybytargetname);//entindex = FindEntityByName(name, entindex)
	f->Declare("FindEntityByClass", new Entitybyclassname);//entindex = FindEntityByClass(classname, entindex)
	f->Declare("IndexOfPlayer", new Playerbyindex);//1 arg (int), returns int
	f->Declare("IsPlayer", new Isplayer);//1 arg (int), returns int
	f->Declare("time", new Time);//0 args, returns double
	f->Declare("max_players", new Maxplayers);//0 args, returns int
	f->Declare("max_ents", new Maxents);//0 args, returns int
	f->Declare("get_entv", new GetPev);//1 arg (int), returns entvars
	f->Declare("put_entv", new PutPev);//2 args (int, entvars), returns 0

	//Game rules
	f->Declare("ReSpawn", new Respawn);//1 arg (int), returns 0
	f->Declare("AllowBuy", new Allowbuy);//1 arg (bool), returns 0
	f->Declare("AddMoney", new Addmoney);//2 args (int, int), returns int
	f->Declare("AddFrags", new Addfrags);//2 args (int, int), returns int
	f->Declare("AddWins", new Addwins);//2 args (int, int), returns int
	f->Declare("GiveWeapon", new GiveWeapon);//2 args (int, string), returns 0
	f->Declare("GiveAmmo", new Giveammo);//3 args (int, string, int), returns 0
	f->Declare("RemoveAll", new StripAll);//2 args (int, bool), returns 0

	//Effects
	f->Declare("LOAD_SOUND", new Precachesound);//1 arg (string), returns 0
	f->Declare("EMIT_SOUND", new PlaySound);//EMIT_SOUND(entindex, str_sample, channel, volume, attenuation)
	f->Declare("EMIT_AMBIENT_SOUND", new PlayAmbSound);//EMIT_AMBIENT_SOUND(origin, str_sample, volume, attenuation)
	f->Declare("ScreenFade", new Screenfade);//ScreenFade (entindex, vector_RGB, fl_time, fl_hold, int_alpha, flags)
	f->Declare("FX_Trail", new Trailfx);//FX_Trail(origin, entindex, type)
	f->Declare("FX_Explosion", new Explfx);//FX_Explosion(origin, type)

	//Additional
	f->Declare("FireBullets", new Firebullets);//FireBullets(vecStart, vecDir, vecSpread, iBulletType, flDist, entindex);
	f->Declare("FireBeam", new Firebeam);//FireBeam(vecStart, vecDir, flDamage, iBeamtype, entindex);
	f->Declare("BOOM", new BOOM);//BOOM!!! (vecSrc, entindex, flDamage, flRadius, bitsDamageType);

	//Object-oriented fake-ass operator:
	f->Declare(MESSAGING_ROUTINE, new Invokation);
}

#pragma warning(default:4800)