#include "../Public/Path.h"
#include "../../Core/Algorithm/Public/StringAlgorithm.h"
#include "../../Core/Array/Public/CArrayString.h"
_CRT_BEGIN_C_HEADER
void GetPathDirectoryInline(char* const PathBuf, size_t PathBufSize)
{
	NormalizationPath(PathBuf);

	char Filename[_MAX_PATH] = { 0 };
	GetPathCleanFilename(Filename, _MAX_PATH, PathBuf);

	RemoveStringStart(PathBuf, Filename);
}

void GetPathDirectory(char* OutBuf, size_t OutBufSize, const char* PathBuf)
{
	strcpy_s(OutBuf, OutBufSize, PathBuf);
	GetPathDirectoryInline(OutBuf, OutBufSize);
}

void GetPathCleanFilename(char* OutBuf, size_t OutBufSize, char const* PathBuf)
{
	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(PathBuf, "/", &CString);

	char* Value = GetCArrayString(CString.size - 1, &CString);
	strcpy_s(OutBuf, OutBufSize, Value);

	DestroyCArrayString(&CString);
}

void GetNormalizationDirectory(char* OutBuf, size_t OutBufSize, char const* PathBuf)
{
	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(PathBuf, "/", &CString);

	for(int i = 0; i < CString.size; i++)
	{
		char* Value = GetCArrayString(i, &CString);
		strcat_s(OutBuf, OutBufSize, Value);
		strcat_s(OutBuf, OutBufSize, "\\");
	}
	DestroyCArrayString(&CString);
}

void NormalizationPath(char* PathBuf)
{
	ReplaceCharAll(PathBuf, '\\', '/');
}
_CRT_END_C_HEADER