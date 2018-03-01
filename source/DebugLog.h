#pragma once

#ifndef __DEBUGLOG_H
#define __DEBUGLOG_H

#if defined(_DEBUG)
#define USE_DEBUG_FILE 1
#endif

#if defined(USE_DEBUG_FILE)

#include "Analyzer.h"

#define FILENAME "D:\\KiIR.log"

enum LogType { DEC = 0, HEX };

extern void LogToFile(const char *pp_text, U64 p_value, LogType pLogType = DEC);
extern void CloseFile(void);

#define LOGTOFILEX(t,v)	LogToFile(t,v,HEX)
#define LOGTOFILE(t,v)	LogToFile(t,v,DEC)
#define CLOSEFILE()		CloseFile()

#else
#define LOGTOFILEX(t,v)
#define LOGTOFILE(t,v)
#define CLOSEFILE()
#endif

#endif
