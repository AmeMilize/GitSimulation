#include "../Public/Guid.h"
#include "../../Core/Algorithm/Public/StringAlgorithm.h"

_CRT_BEGIN_C_HEADER
int HexDigit(char c)
{
	int ret = 0;
	if(c >= '0' && c <= '9')
	{
		ret = c - '0';
	}
	else if(c >= 'A' && c <= 'F')
	{
		ret = c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f')
	{
		ret = c - 'a' + 10;
	}
	return ret;
}


unsigned long long HexNumber(char* Buf)
{
	unsigned long long ret = 0;
	while(*Buf)
	{
		ret <<= 4;
		ret += HexDigit(*Buf++);
	}
	return ret;
}

void InitGUID(GUIDStruct* OriginGUID)
{
	OriginGUID->a = 0ul;
	OriginGUID->b = (unsigned short)0;
	OriginGUID->c = (unsigned short)0;
	OriginGUID->d = 0ull;
}

bool CreateGUID(GUIDStruct* OriginGUID)
{
	InitGUID(OriginGUID);
	return CoCreateGuid((GUIDStruct*)OriginGUID) == S_OK;
}

void GUIDToString(char* Buf, size_t BufSize, GUIDStruct const* const OriginGUID)
{

	unsigned short sd1 = (unsigned short)(OriginGUID->d >> 48);
	unsigned short sd2 = (unsigned short)((OriginGUID->d << 16) >> 48);
	unsigned long  ld  = (unsigned long) ((OriginGUID->d << 32) >> 32);

	GetFormatStringS(Buf, BufSize, "%08X-%04X-%04X-%04X-%04X%08X", OriginGUID->a, OriginGUID->b, OriginGUID->c, sd1, sd2, ld);
}

void StringToGUID(GUIDStruct* OutGuid, char const* InBuffer)
{
	if (strlen(InBuffer) + 1 >= 36)
	{
		char StrChipBuf[MAX_PATH] = { 0 };
		OutGuid->a = (unsigned long)HexNumber(StringCat(StrChipBuf, 9, InBuffer, 8));
		OutGuid->b = (unsigned short)HexNumber(StringCat(StrChipBuf, 5, InBuffer + 9, 4));
		OutGuid->c = (unsigned short)HexNumber(StringCat(StrChipBuf, 5, InBuffer + 14, 4));
		OutGuid->d = HexNumber(StringCat(StrChipBuf, 5, InBuffer + 19, 4));
		OutGuid->d <<= 48;
		OutGuid->d += HexNumber(StringCat(StrChipBuf, 13, InBuffer + 24, 12));
	}
}

bool GuidEqualString(char const* GuidAString, GUIDStruct const* GuidB)
{
	GUIDStruct GuidA;
	StringToGUID(&GuidA, GuidAString);

	return GuidEqual(&GuidA, GuidB);
}

bool GuidEqual(GUIDStruct* GuidA, GUIDStruct* GuidB)
{
	return  GuidA->a == GuidB->a &&
			GuidA->b == GuidB->b &&
			GuidA->c == GuidB->c &&
			GuidA->d == GuidB->d;
}
_CRT_END_C_HEADER