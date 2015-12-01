// shit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "stdio.h"

#include "parser.h"
#include "kernel.h"
#include "error.h"

using namespace std;

GlobalScope globals;				//SERVER global vars

ScriptScope properties(&globals);	//ENTITY1 properties
Functions func;						//entity1 methods list

extern int errors;					//are there any errors in script?

int main()
{
	cout<<"Enter filename (i.e. scripts/script.txt): ";

	char path[280];

	cin>>path;

	cout<<"Checking "<<path<<"...\n";

	Constants *constants = new Constants;
	StructData *structs = new StructData;

	CreateKernel (&func);

	ParseFile (path, &globals, &properties, constants, &func, structs);

	delete structs;
	delete constants;

	properties.EndRegistration();

	if (func.Completed())
	{
		func.EndRegistration();
	}
	else
	{
		ErrorMessage ("Unsatistfied forward declaration(s) found");
	}

	if (!errors)
		cout<<"Everything is OK\n";

	int x;

	cin>>x;

	return 0;
}
