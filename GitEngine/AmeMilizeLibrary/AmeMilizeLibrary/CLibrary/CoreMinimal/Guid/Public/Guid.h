#pragma once

#include "../../Core/CoreMinimal.h"


_CRT_BEGIN_C_HEADER
typedef struct
{
	unsigned long      a;
	unsigned short     b;
	unsigned short     c;
	unsigned long long d;

} GUIDStruct;
void InitGUID(GUIDStruct* OriginGUID);
void GUIDToString(char* Buf, size_t BufSize, GUIDStruct const* const OriginGUID);
void StringToGUID(GUIDStruct* OutGuid, char const* InBuffer);
bool CreateGUID(GUIDStruct* OriginGUID);
bool GuidEqual(GUIDStruct const* GuidA, GUIDStruct const* GuidB);
bool GuidEqualString(char const* GuidAString, GUIDStruct const* GuidB);
_CRT_END_C_HEADER