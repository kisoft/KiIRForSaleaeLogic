#include "DebugLog.h"

#if defined(_DEBUG)

#include <stdio.h>
#include <LogicPublicTypes.h>

FILE *f = nullptr;

void LogToFile(const char *pp_text, U64 p_value, LogType pLogType /*= DEC*/)
{
	if (!f)
	{
		fopen_s(&f, FILENAME, "wt");
	}
	if (HEX == pLogType)
	{
		fprintf(f, "%s: %llX\n", pp_text, p_value);
	}
	else
	{
		fprintf(f, "%s: %llu\n", pp_text, p_value);
	}
	fflush(f);
}

void CloseFile(void)
{
	if (f)
	{
		fclose(f);
		f = nullptr;
	}
}

#endif
