#include "../Public/CArrayString.h"
#include "../../Algorithm/Public/StringAlgorithm.h"
_CRT_BEGIN_C_HEADER
void InitCArrayString(CArrayString* CArrayPtr)
{
	assert(CArrayPtr);
	CArrayPtr->size = 0;
	CArrayPtr->data = NULL;
}

void DestroyCArrayString(CArrayString* const CArrayPtr)
{
	assert(CArrayPtr);
	free(CArrayPtr->data);
}

void AddCArrayString(char const* NewData, CArrayString* CArrayPtr)
{
	assert(CArrayPtr);

	int index = CArrayPtr->size;
	CArrayPtr->size++;
	CArrayPtr->data = (StringNode*)realloc(CArrayPtr->data, CArrayPtr->size * sizeof(StringNode));
	strcpy_s(CArrayPtr->data[index].data, sizeof(CArrayPtr->data[index].data), NewData);
}

char* GetCArrayString(int Index, CArrayString* CArrayPtr)
{
	return CArrayPtr->data[Index].data;
}

void DismantlingCArrayString(char const* const InData, const char* StrSub, CArrayString* const CArrayPtr)
{
	char InDataStore[8192] = { 0 };
	strcpy_s(InDataStore, 8192, InData);

	char* ContextPtr = NULL;
	char* p = NULL;

	p = strtok_s(InDataStore, StrSub, &ContextPtr);
	while(p)
	{
		AddCArrayString(p, CArrayPtr);
		p = strtok_s(NULL, StrSub, &ContextPtr);
	}
}

char* DismantlingStringByIndex(int index, char const* SplitStr, char const* SubStr, char* Buffer, size_t BufferSize)
{
	CArrayString CStringParam;
	InitCArrayString(&CStringParam);

	DismantlingCArrayString(SplitStr, SubStr, &CStringParam);
	char* Ptr = GetCArrayString(index, &CStringParam);
	RemoveCharEnd(Ptr, '\n');

	strcpy_s(Buffer, BufferSize, Ptr);
	DestroyCArrayString(&CStringParam);

	return Buffer;
}

bool IsExist(char const* Indata, CArrayString* CArrayPtr)
{
	for(int i = 0; i < CArrayPtr->size; i++)
	{
		if(strcmp(CArrayPtr->data[i].data, Indata) == 0)
		{
			return true;
		}
	}
	return false;
}
_CRT_END_C_HEADER