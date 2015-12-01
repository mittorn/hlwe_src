//================================== ERROR ======================================================

#include "stdafx.h"
#include "error.h"

#include <iostream>

int errors = 0;

void ErrorMessage ( char *message )
{
	std::cerr << "ERROR: " << message << "\n";
	++errors;
}

void PrintSequence ( char *begin, char *end )
{
	std::cout<<"\"";

	for (char *c = begin; c < end; std::cout<<(*(c++)));

	std::cout<<"\"\n";
}