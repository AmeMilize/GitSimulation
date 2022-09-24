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


void InitCArrayString(CArrayString* CArrayPtr); //��ʼ�� �����ڴ�
void DestroyCArrayString(CArrayString* CArrayPtr); //�����ڵ�
void AddCArrayString(char* NewData, CArrayString* CArrayPtr); //���Ԫ��
char* GetCArrayString(int Index, CArrayString* CArrayPtr);
void DismantlingCArrayString(char const* const InData, const char* StrSub, CArrayString* const CArrayPtr);
char* DismantlingStringByIndex(int index, char const* SplitStr, char const* SubStr, char* Buffer, size_t BufferSize);
bool IsExist(char const* Indata, CArrayString* CArrayPtr);
_CRT_END_C_HEADER