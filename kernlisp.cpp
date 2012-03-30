// kernlisp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern "C" {
void initialize();
void topexec();
}

int _tmain(int argc, _TCHAR* argv[])
{
	initialize();
	topexec();
	return 0;
}

