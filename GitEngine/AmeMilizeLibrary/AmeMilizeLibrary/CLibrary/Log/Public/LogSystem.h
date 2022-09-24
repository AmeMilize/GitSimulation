#pragma once
#include "../../CoreMinimal/Core/CoreMinimal.h"

#define TIME_BUFFER_SIZE 256
#define LOG_SUCCESS(...) WriteLog(ELOG_SUCCESS, __VA_ARGS__)
#define LOG_GENERAL(...) WriteLog(ELOG_GENERAL, __VA_ARGS__)
#define LOG_WARNING(...) WriteLog(ELOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   WriteLog(ELOG_ERROR, __VA_ARGS__)

typedef enum LogEnum
{
	ELOG_SUCCESS = 0,
	ELOG_GENERAL,
	ELOG_WARNING,
	ELOG_ERROR

} LogEnum;

_CRT_BEGIN_C_HEADER
void InitLogSystem(char const* NewLogPath);
char const* GetLogPath(void);
char const* GetLogFileName(void);
void GetTime(char** TimeBuffer);
void WriteLog(LogEnum LogType, char* Format, ...);
_CRT_END_C_HEADER
