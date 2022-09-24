#pragma once

#include "../../CoreMinimal.h"


_CRT_BEGIN_C_HEADER
typedef struct StringNode
{
	char data[8192];

} StringNode;

typedef struct CArrayString
{
	int size;
	StringNode* data;
} CArrayString;


void InitCArrayString(CArrayString* CArrayPtr); //初始化 分配内存
void DestroyCArrayString(CArrayString* CArrayPtr); //消除节点
void AddCArrayString(char* NewData, CArrayString* CArrayPtr); //添加元素
char* GetCArrayString(int Index, CArrayString* CArrayPtr);
void DismantlingCArrayString(char const* const InData, const char* StrSub, CArrayString* const CArrayPtr);
char* DismantlingStringByIndex(int index, char const* SplitStr, char const* SubStr, char* Buffer, size_t BufferSize);
bool IsExist(char const* Indata, CArrayString* CArrayPtr);
_CRT_END_C_HEADER