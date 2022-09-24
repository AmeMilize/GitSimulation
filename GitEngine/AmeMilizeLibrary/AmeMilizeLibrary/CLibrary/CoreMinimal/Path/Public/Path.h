#pragma once
#include "../../Core/CoreMinimal.h"

_CRT_BEGIN_C_HEADER
void GetPathDirectoryInline(char* const PathBuf, size_t PathBufSize);
void GetPathDirectory(char* OutBuf, size_t OutBufSize, const char* PathBuf);
void GetPathCleanFilename(char* OutBuf, size_t OutBufSize, char const* PathBuf);
void GetNormalizationDirectory(char* OutBuf, size_t OutBufSize, char const* PathBuf);
void NormalizationPath(char* PathBuf);
_CRT_END_C_HEADER