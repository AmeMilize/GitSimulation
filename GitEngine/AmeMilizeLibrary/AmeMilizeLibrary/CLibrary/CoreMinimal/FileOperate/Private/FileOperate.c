#include "../Public/FileOperate.h"
#include "../../Core/Array/Public/CArrayString.h"
_CRT_BEGIN_C_HEADER
bool AddFileBuf(char const* const FileName, char const* const Buffer)
{
	FILE* FilePtr = NULL;

	if ((fopen_s(&FilePtr, FileName, "a+")) == 0)
	{
		bool bRet = fprintf(FilePtr, "%s", Buffer);

		fclose(FilePtr);
		return true;
	}

	return false;
}

bool AddNewFileBuf(char const* FileName, char* Buffer)
{
	FILE* FilePtr = NULL;

	if ((fopen_s(&FilePtr, FileName, "w+")) == 0)
	{
		bool bRet = fprintf(FilePtr, "%s", Buffer);

		fclose(FilePtr);
		return true;
	}

	return false;
}

int CCopyFile(char const* Src, char const* Dest)
{
	char buf[BUF_SIZE] = { 0 };
	int FileSize = 0;

	FILE* SrcFile = NULL;
	FILE* DestFile = NULL;

	if (fopen_s(&SrcFile, Src, "rb") != 1)
	{
		if (fopen_s(&DestFile, Dest, "wb") != 1)
		{
			while ((FileSize = fread(buf, sizeof(char), sizeof(buf), SrcFile)) != 0)
			{
				fwrite(buf, FileSize, sizeof(char), DestFile);
				memset(buf, 0, sizeof(buf));
			}

			fclose(DestFile);
		}
		fclose(SrcFile);
		return true;
	}
	return false;
}

bool CCreateFile(const char* const FileName)
{
	FILE* FilePtr = NULL;

	if ((fopen_s(&FilePtr, FileName, "w")) == 0)
	{
		fclose(FilePtr);
		return true;
	}

	return false;
}

bool CreateFileDirectory(char* Src)
{
	CArrayString CFile;
	InitCArrayString(&CFile);
	if (strstr(Src, "\\"))
	{
		DismantlingCArrayString(Src, "\\", &CFile);
	}
	else if (strstr(Src, "/"))
	{
		DismantlingCArrayString(Src, "/", &CFile);
	}

	char path[_MAX_PATH] = { 0 };
	for (int i = 0; i < CFile.size; i++)
	{
		char* value = GetCArrayString(i, &CFile);
		strcat_s(path, _MAX_PATH, value);
		strcat_s(path, _MAX_PATH, "\\");
		if (_access(path, 0) == -1)
		{
			_mkdir(path);
		}
	}
	DestroyCArrayString(&CFile);

	return !_access(path, 0);
}

void InitPathCollection(PathCollection* const PathColPtr)
{
	PathColPtr->index = 0;
	memset(PathColPtr->paths, 0, sizeof(PathColPtr->paths));
}

void FindFiles(char const* path, PathCollection* PathColPtr, bool bRecursion)
{
	struct _finddata_t FindData;
	intptr_t HFile = 0;
	char TmpPath[_MAX_PATH] = { 0 };

	strcpy_s(TmpPath, sizeof(TmpPath), path);
	strcat_s(TmpPath, sizeof(TmpPath), "\\*.*");

	if ((HFile = _findfirst(TmpPath, &FindData)) != HANDLE_NONE)
	{
		do
		{
			if (FindData.attrib & _A_SUBDIR) //是不是目录
			{
				if (bRecursion) {
					if (strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0) continue;
					else
					{
						char NewPath[_MAX_PATH] = { 0 };
						strcpy_s(NewPath, sizeof(NewPath), path);
						strcat_s(NewPath, sizeof(NewPath), "\\");
						strcat_s(NewPath, sizeof(NewPath), FindData.name);

						FindFiles(NewPath, PathColPtr, bRecursion);
					}
				}
			}
			else
			{
				if (FindData.attrib & _A_NORMAL) printf("_A_NORMAL \r\n");
				if (FindData.attrib & _A_RDONLY) printf("_A_RDONLY \r\n");
				if (FindData.attrib & _A_HIDDEN) printf("_A_HIDDEN \r\n");
				if (FindData.attrib & _A_SYSTEM) printf("_A_SYSTEM \r\n");
				if (FindData.attrib & _A_ARCH) printf("_A_ARCH \r\n");


				strcpy_s(PathColPtr->paths[PathColPtr->index], sizeof(PathColPtr->paths[PathColPtr->index]), path);
				strcat_s(PathColPtr->paths[PathColPtr->index], sizeof(PathColPtr->paths[PathColPtr->index]), "\\");
				strcat_s(PathColPtr->paths[PathColPtr->index++], sizeof(PathColPtr->paths[PathColPtr->index]), FindData.name);
			}
		} while (_findnext(HFile, &FindData) == 0);
		_findclose(HFile);
	}
}

bool GetFileBuf(char const* FileName, char* Buffer, size_t BufSize)
{
	FILE* FilePtr = NULL;

	if ((fopen_s(&FilePtr, FileName, "r")) == 0)
	{
		char TmpReadBuf[512] = { 0 };
		size_t FileSize = 0;
		while ((FileSize = fread_s(TmpReadBuf, 512, sizeof(char), 512, FilePtr)) > 0)
		{
			strcat_s(Buffer, BufSize, TmpReadBuf);
			memset(TmpReadBuf, 0, 512);
		}
		fclose(FilePtr);

		return Buffer[0] != '\0';
	}

	return false;
}

unsigned GetFileSizeByFileName(char const* const Filename)
{
	unsigned int FileSize = 0;

	FILE* FilePtr = NULL;
	if (fopen_s(&FilePtr, Filename, "r") == 0)
	{
		FileSize = GetFileSizeByHandle(FilePtr);

		fclose(FilePtr);
	}

	return FileSize;
}

unsigned int GetFileSizeByHandle(FILE* FileHandle)
{
	unsigned int FileSize = 0;


	unsigned int StartReadPosition = ftell(FileHandle);

	fseek(FileHandle, 0, SEEK_END);

	FileSize = ftell(FileHandle) - StartReadPosition;

	fseek(FileHandle, StartReadPosition, SEEK_SET);

	return FileSize;
}
_CRT_END_C_HEADER