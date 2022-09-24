#include "../Public/StringAlgorithm.h"
_CRT_BEGIN_C_HEADER
int FindString(char const* const Str, char const* SubStr)
{
	int const len = (int)strlen(SubStr);
	int index = INDEX_NONE;
	for(int i = 0; Str[i] != '\0'; i++)
	{
		if(SubStr[0] == Str[i])
		{
			int MatchLength = 1;
			while (SubStr[MatchLength] != '\0' && Str[i + MatchLength] != '\0' && (SubStr[MatchLength] == Str[i + MatchLength]))
			{
				MatchLength++;
			}

			if(len == MatchLength)
			{
				index = i;
				break;
			}
		}
	}
	return index;
}

void RemoveStringStart(char* const Str, char const* const SubStr)
{
	size_t const StrLength = strlen(Str);
	int const index = FindString(Str, SubStr);
	if(index != INDEX_NONE)
	{
		strcpy_s(&Str[index], StrLength - index, &Str[index + strlen(SubStr)]);
	}
}

void RemoveCharStart(char* const Str, char const SubChar)
{
	size_t const StrLength = strlen(Str);
	for(int i = 0; i < (int)StrLength; i++)
	{
		if(Str[i] == SubChar)
		{
			strcpy_s(&Str[i], StrLength - i, &Str[i + 1]);
			break;
		}
	}
}

void RemoveCharEnd(char* const Str, char const SubChar)
{
	size_t const StrLength = strlen(Str);
	for (int i = (int)StrLength - 1; i >= 0; i--)
	{
		if (Str[i] == SubChar)
		{
			strcpy_s(&Str[i], StrLength - i, &Str[i + 1]);
			break;
		}
	}
}

void RemoveCharEndAll(char* const Str, char const SubChar)
{
	size_t const StrLength = strlen(Str);
	for (int i = (int)StrLength - 1; i >= 0; i--)
	{
		if (Str[i] == SubChar)
		{
			strcpy_s(&Str[i], StrLength - i, &Str[i + 1]);
		}
	}
}

void ReplaceCharAll(char* const Str, char const SubCharOld, char const SubCharNew)
{
	int const index = FindChar(Str, SubCharOld);
	if(index != INDEX_NONE)
	{
		Str[index] = SubCharNew;
		ReplaceCharAll(Str, SubCharOld, SubCharNew);
	}
}

int FindChar(char const* const Str, char const SubChar)
{
	int index = INDEX_NONE;
	for (int i = 0; Str[i] != '\0'; i++)
	{
		if (Str[i] == SubChar)
		{
			index = i;
		}
	}
	return index;
}

int GetFormatStringS(char* const OutBuf, size_t OutBufSize, char const* Format, ...)
{
	char Buf[1024] = { 0 };

	va_list Args;
	va_start(Args, Format);

	_vsnprintf_s(Buf, 1024, 1023, Format, Args);
	va_end(Args);
	Buf[1024 - 1] = '\0';

	strcpy_s(OutBuf, OutBufSize, Buf);
	return (int)strlen(OutBuf) + 1;
}

int GetFormatString(char* const Buf, char const* Format, ...)
{
	va_list ParamList;
	va_start(ParamList, Format);

	char ParamChar = *Format++;
	int RetLen = 0;

	for (int i = 0; ParamChar != '\0'; i++)
	{
		if (ParamChar != '%')
		{
			Buf[RetLen++] = ParamChar;
		}
		else
		{
			ParamChar = *Format++;
			switch (ParamChar)
			{
			case 'c':
			{
				Buf[RetLen++] = va_arg(ParamList, char);
				break;
			}
			case 's':
			{
				for (char const* SPtr = va_arg(ParamList, char*); *SPtr != '\0'; SPtr++)
					Buf[RetLen++] = *SPtr;
				break;
			}
			case 'd':
			{
				char BufInt[10] = { 0 };
				int i_int = va_arg(ParamList, int);
				_itoa_s(i_int, BufInt, sizeof(BufInt), 10);
				int i_len = strlen(BufInt);
				for (int j = 0; j < i_len; j++)
					Buf[RetLen++] = BufInt[j];
				break;
			}
			case '%':
			{
				Buf[RetLen++] = '%';
				break;
			}
			}

		}
		ParamChar = *Format++;
	}
	return RetLen;
}

char* StringCat(char* const OutBuf, int OutSize, char const* const IntBuf, int StrLength)
{
	memcpy_s(OutBuf, OutSize, IntBuf, StrLength);
	OutBuf[OutSize - 1] = '\0';
	return OutBuf;
}
_CRT_END_C_HEADER