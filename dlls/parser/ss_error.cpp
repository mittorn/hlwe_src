//================================== ERROR ======================================================
#include "extdll.h"
#include "enginecallback.h"
#include "ss_error.h"
#include "string.h"

int errors = 0;

#include "stdio.h"

void ErrorMessage ( const char *message )
{
	char path[MAX_PATH];

	GET_GAME_DIR(path);

	strcat(path, "/ss_errlog.txt");

	FILE* pf = fopen(path, "a");

	if (pf)
	{
		fprintf(pf, "ERROR: %s\n", message);
		fclose(pf);
	}

	++errors;
}

void PrintSequence ( const char *begin, const char *end )
{
	char path[MAX_PATH];

	GET_GAME_DIR(path);

	strcat(path, "/ss_errlog.txt");

	FILE* pf = fopen(path, "a");

	if (pf)
	{
		fputc('\"', pf);
		for (char *c = (char*)begin; c < end; fputc(*(c++), pf));
		fprintf(pf, "\"\n");

		fclose(pf);
	}
}