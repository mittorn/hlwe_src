/*===============================================================================================

	Kernel of Samogon Script - all those functions, that linking Weapon Edition engine and
	script file.

===============================================================================================*/

#include "StdAfx.h"

#include "functions.h"
#include "kernel.h"
#include "keywords.h"

#include "math.h"
#include "string.h"


//============================================
//	Array type constructor
//============================================
class Array : public KernelFunction
{
public:

	Array () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


//============================================
//	Basic mathematical functions of kernel
//============================================
class Cos : public KernelFunction
{
public:

	Cos () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
	};
};

class Sin : public KernelFunction
{
public:

	Sin () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Asin : public KernelFunction
{
public:

	Asin () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Acos : public KernelFunction
{
public:

	Acos () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Atan : public KernelFunction
{
public:

	Atan () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Log : public KernelFunction
{
public:

	Log () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Pow : public KernelFunction
{

public:

	Pow () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Ceil : public KernelFunction
{
public:

	Ceil () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{
	};
};


class Floor : public KernelFunction
{
public:

	Floor () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Strlen : public KernelFunction
{
public:

	Strlen () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Substr : public KernelFunction
{
public:

	Substr () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Insert : public KernelFunction
{
public:

	Insert () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Delete : public KernelFunction
{
public:

	Delete () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Arraysize : public KernelFunction
{
public:

	Arraysize () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


class Random : public KernelFunction
{
public:

	Random () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Numtostr : public KernelFunction
{
public:

	Numtostr () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return args_constant;
	};

	void PreExecute ()
	{

	};
};


//============================================
//	Basic i/o functions of kernel
//============================================
class Write : public KernelFunction
{
public:

	Write () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


//====================================
//Link samogon scripts with
//Weapon Edition engine
//====================================
class Kill : public KernelFunction
{
public:

	Kill () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Damage : public KernelFunction
{
public:

	Damage () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Trigger : public KernelFunction
{
public:

	Trigger () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Traceline : public KernelFunction
{
public:

	Traceline () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class CreateScripted : public KernelFunction
{
public:

	CreateScripted () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Setctl : public KernelFunction
{
public:

	Setctl () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Precachemodel : public KernelFunction
{
public:

	Precachemodel () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Precachesound : public KernelFunction
{
public:

	Precachesound () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Setmodel : public KernelFunction
{
public:

	Setmodel () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Setsize : public KernelFunction
{
public:

	Setsize () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Getcenter : public KernelFunction
{
public:

	Getcenter () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Isplayer : public KernelFunction
{
public:

	Isplayer () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Playerbyindex : public KernelFunction
{
public:

	Playerbyindex () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Entitybyclassname : public KernelFunction
{
public:

	Entitybyclassname () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Entitybytargetname : public KernelFunction
{
public:

	Entitybytargetname () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Entityinsphere : public KernelFunction
{
public:

	Entityinsphere () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Time : public KernelFunction
{
public:

	Time () : KernelFunction(0) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Maxplayers : public KernelFunction
{
public:

	Maxplayers () : KernelFunction(0) {};

	void PassArg ( unsigned int argi, Type argv ){};

	Type Execute ()
	{
		return _ZERO;
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
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute (){};
};


class GetPev : public KernelFunction
{
public:

	GetPev () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};

class PutPev : public KernelFunction
{
public:

	PutPev () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


//====================================
//	Game rules management
//====================================
class Respawn : public KernelFunction
{
public:

	Respawn () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Allowbuy : public KernelFunction
{
public:

	Allowbuy () : KernelFunction(1) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};

class Addmoney : public KernelFunction
{
public:

	Addmoney () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Addfrags : public KernelFunction
{
public:

	Addfrags () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Addwins : public KernelFunction
{
public:

	Addwins () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};

class GiveWeapon : public KernelFunction
{
public:

	GiveWeapon () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Giveammo : public KernelFunction
{
public:

	Giveammo () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class StripAll : public KernelFunction
{
public:

	StripAll () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


//====================================
// Simple game effects creation
//====================================
class Screenfade : public KernelFunction
{
public:

	Screenfade () : KernelFunction(6) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class PlaySound : public KernelFunction
{
public:

	PlaySound () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class PlayAmbSound : public KernelFunction
{
public:

	PlayAmbSound () : KernelFunction(4) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Trailfx : public KernelFunction
{
public:

	Trailfx () : KernelFunction(3) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Explfx : public KernelFunction
{
public:

	Explfx () : KernelFunction(2) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};



//====================================
//	Additional functions
//====================================
class Firebullets : public KernelFunction
{
public:

	Firebullets () : KernelFunction(6) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};


class Firebeam : public KernelFunction
{
public:

	Firebeam () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

	};
};



class BOOM : public KernelFunction
{
public:

	BOOM () : KernelFunction(5) {};

	void PassArg ( unsigned int argi, Type argv )
	{

	};

	Type Execute ()
	{
		return _ZERO;
	};

	bool Constant ( bool args_constant )
	{
		return false;
	};

	void PreExecute ()
	{

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
}