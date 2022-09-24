#pragma once

#include "../../../Core/CoreMinimal.h"


#define REG_SHELL_DIRECTORY     "\\Shell\\"                             //在对象上鼠标右键，添加按钮
#define REG_DIRECTORY           "Directory\\Background\\shell\\"        //空白处鼠标右键，添加按钮
#define REG_DESKTOPBACKGROUND   "DesktopBackground\\shell\\"            //在桌面上鼠标右键，添加按钮
#define REG_PATH_TEST REG_DIRECTORY

_CRT_BEGIN_C_HEADER
typedef struct
{
    DWORD ValueType;
    char ValueName[_MAX_PATH];
    char Buf[1024];
} FregValue;

typedef struct
{
    char Filename[_MAX_PATH]; //项路径名
    HKEY HKey;

    int ValueCount; //项内值总数
    FregValue ValueCollection[50];
} FregInfo;

void InitFregInfo(FregInfo* const Info);
bool RegisterInfo(FregInfo const* const Info);
bool DeleteRegisteredInfo(HKEY HKey, char const* Filename);
bool DeleteRegisteredKey(HKEY HKey, const char* Filename, const char* SubkeyName);
bool CreateButton(char const* ExePath, char const* IconPath, char const* ButtonName);
bool DestroyButton(char const* ButtonName);
_CRT_END_C_HEADER