// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

int main(int argc, char* argv[])
{
	return WinExec ("hl.exe -game hl15we -console", 0);//declared in "winbase.h"
}

