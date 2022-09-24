#pragma once

#include "../../Core/CoreMinimal.h"

#define BUF_SIZE 512
#define HANDLE_NONE -1

_CRT_BEGIN_C_HEADER
typedef struct AllPath
{
	int index;
	char paths[1024][_MAX_PATH];
} PathCollection;
void InitPathCollection(PathCollection* const PathColPtr);
void FindFiles(char const* path, PathCollection* PathColPtr, bool bRecursion);

int CCopyFile(char* Src, char* Dest);
bool CCreateFile(char const* const FileName);
bool CreateFileDirectory(char* Src);
bool GetFileBuf(char const* FileName, char* Buffer, size_t BufSize);
bool AddFileBuf(char const* const FileName, char const* const Buffer);
bool AddNewFileBuf(char const* FileName, char* Buffer);
unsigned int GetFileSizeByFileName(char const* const Filename);
unsigned int GetFileSizeByHandle(FILE* FileHandle);
_CRT_END_C_HEADER