#pragma once
#include "../../../CoreMinimal.h"

#define MAX 10;

#define CARRAY_GENERAL_STRUCT(TypeName, Type) \
typedef struct CArray##TypeName \
{ \
	int size; \
	int MaxSize; \
	Type* data; \
} CArray##TypeName;

#define CARRAY_GENERAL(TypeName, Type) \
CARRAY_GENERAL_STRUCT(TypeName, Type) \
void InitCArray##TypeName(CArray##TypeName* CArrayPtr) \
{ \
	assert(CArrayPtr); \
\
	CArrayPtr->size = 0; \
	CArrayPtr->MaxSize = MAX; \
	CArrayPtr->data = (Type*)malloc(CArrayPtr->MaxSize * sizeof(Type)); \
	memset(CArrayPtr->data, 0, CArrayPtr->MaxSize); \
} \
\
void DestroyCArray##TypeName(CArray##TypeName* CArrayPtr) \
{ \
	assert(CArrayPtr); \
	free(CArrayPtr->data); \
} \
\
void AddCArray##TypeName(Type const NewData, CArray##TypeName* CArrayPtr) \
{ \
	assert(CArrayPtr); \
\
	if (CArrayPtr->size == CArrayPtr->MaxSize) \
	{ \
		CArrayPtr->MaxSize += MAX; \
		CArrayPtr->data = (Type*)realloc(CArrayPtr->data, CArrayPtr->MaxSize * sizeof(Type)); \
	} \
\
	CArrayPtr->data[CArrayPtr->size] = NewData; \
	CArrayPtr->size++; \
}