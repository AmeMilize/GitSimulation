#pragma once

#include "../../CoreMinimal.h"
#define INDEX_NONE -1


_CRT_BEGIN_C_HEADER
void RemoveStringStart(char* const Str, char const* const SubStr);
void RemoveCharStart(char* const Str, char const SubChar);
void RemoveCharEnd(char* const Str, char const SubChar);
void RemoveCharEndAll(char* const Str, char const SubChar);
void ReplaceCharAll(char* const Str, char const SubCharOld, char const SubCharNew);
int FindChar(char const* const Str, char const SubChar);
int FindString(char const* const Str, char const* SubStr);
int GetFormatStringS(char* const OutBuf, size_t OutBufSize, char const* Format, ...);
int GetFormatString(char* const Buf, char const* Format, ...);
char* StringCat(char* const OutBuf, int OutSize, char const* const IntBuf, int StrLength);
_CRT_END_C_HEADER