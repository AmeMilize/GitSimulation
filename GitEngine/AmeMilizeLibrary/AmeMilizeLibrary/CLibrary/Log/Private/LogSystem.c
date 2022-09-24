#define _CRT_SECURE_NO_WARNINGS

#include "../Public/LogSystem.h"

#include "../../CoreMinimal/Windows/Command/Public/WindowsSettings.h"
#include "../../CoreMinimal/Core/Algorithm/Public/StringAlgorithm.h"

char LogPath[_MAX_PATH] = { 0 };
char LogFileName[_MAX_PATH] = { 0 };

_CRT_BEGIN_C_HEADER
void InitLogSystem(char const* NewLogPath)
{
	strcpy(LogPath, NewLogPath);
	_mkdir(LogPath);
}

char const* GetLogPath()
{
	return LogPath;
}

char const* GetLogFileName(void)
{
	if (LogFileName[0] == '\0')
	{
		if ((*GetLogPath()) == '\0')
		{
			SetWindowFontRepresentColor(RED, BLACK);
			printf("\n You need to add the log path in InitLogSystem() first. \n");
			SetWindowFontRepresentColor(WHITE, BLACK);
			assert(0);
		}

		time_t TimeNow = time(NULL);
		struct tm const* const TimeStruct = localtime(&TimeNow);

		char TmpPath[_MAX_PATH] = { 0 };
		sprintf(TmpPath, "%s\\%d%02d%02d.txt", GetLogPath(), TimeStruct->tm_year + 1900, TimeStruct->tm_mon + 1, TimeStruct->tm_mday);

		FILE* HFile = NULL;
		if ((HFile = fopen(TmpPath, "a+")) != NULL)
		{
			strcpy(LogFileName, TmpPath);
			fclose(HFile);
		}
	}

	return LogFileName;
}

void GetTime(char** TimeBuffer)
{
	time_t const TimeNow = time(NULL);
	*TimeBuffer = ctime(&TimeNow);
	RemoveCharEnd(*TimeBuffer, '\n');
}

void WriteLog(LogEnum LogType, char* Format, ...)
{
	char LogTypString[64] = { 0 };
	switch (LogType)
	{
	case ELOG_SUCCESS:
	{
		SetWindowFontRepresentColor(PALE_GREEN, BLACK);
		strcpy(LogTypString, "SUCCESS");
		break;
	}
	case ELOG_GENERAL:
	{
		SetWindowFontRepresentColor(WHITE, BLACK);
		strcpy(LogTypString, "LOG");
		break;
	}
	case ELOG_WARNING:
	{
		SetWindowFontRepresentColor(YELLOW, BLACK);
		strcpy(LogTypString, "WARNING");
		break;
	}
	case ELOG_ERROR:
	{
		SetWindowFontRepresentColor(RED, BLACK);
		strcpy(LogTypString, "ERROR");
		break;
	}
	}

	char TextBuf[1024] = { 0 };
	va_list ParamList;
	va_start(ParamList, Format);

	_vsnprintf(TextBuf, 1024, Format, ParamList);
	TextBuf[1023] = '\0';

	va_end(ParamList);

	char* TimeBuffer = NULL;
	GetTime(&TimeBuffer);

	char FinalInfoBuffer[1024] = { 0 };
	GetFormatString(FinalInfoBuffer, "[%s][%s] %s \r\n", LogTypString, TimeBuffer, TextBuf);

	FILE* HFile = NULL;
	if ((HFile = fopen(GetLogFileName(), "a+")) != NULL)
	{
		printf("%s", FinalInfoBuffer);
		fprintf(HFile, "%s", FinalInfoBuffer);
		fclose(HFile);
	}
	SetWindowFontRepresentColor(WHITE, BLACK);
}
_CRT_END_C_HEADER