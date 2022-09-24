#include "../Public/Registry.h"

_CRT_BEGIN_C_HEADER
void InitFregInfo(FregInfo* const Info)
{
    Info->ValueCount = 0;
    memset(Info->Filename, 0, sizeof(Info->Filename));
    memset(Info->ValueCollection, 0, sizeof(Info->ValueCollection));
}

bool RegisterInfo(FregInfo const* const Info)
{
    /*
    REG_OPTION_NON_VOLATILE 重新启动不销毁 REG_OPTION_VOLATILE 重新启动自动销毁
    KEY_CREATE_SUB_KEY 支持创建子键 KEY_ALL_ACCESS 可以访问
    ERROR_SUCCESS 代表成功
    */

    HKEY HResult; //存储返回的句柄
    long ErrorValue = -1L;
    if ((ErrorValue = RegCreateKeyEx(Info->HKey, Info->Filename, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &HResult, NULL)) == ERROR_SUCCESS)
    {
        for (int i = 0; i < Info->ValueCount; i++)
        {
            const char* ValueName = (Info->ValueCollection[i].ValueName[0] == '\0') ? NULL : Info->ValueCollection[i].ValueName;
            if ((ErrorValue = RegSetValueEx(HResult, ValueName, 0, Info->ValueCollection[i].ValueType, (BYTE*)Info->ValueCollection[i].Buf, sizeof(Info->ValueCollection[i].Buf))) != ERROR_SUCCESS)
            {
                printf("\n error code RegSetValueEx = %ld \n", ErrorValue);
            }
        }

        RegCloseKey(HResult);
        return true;
    }
    else
    {
        printf("\n error code RegCreateKeyEx = %ld \n", ErrorValue);
    }

    return false;
}

bool DeleteRegisteredInfo(HKEY HKey, char const* Filename)
{
    int ErrorCount = 0;

    HKEY HResult;
    long ErrorValue = -1;
    DWORD count;
    if ((ErrorValue = RegOpenKeyEx(HKey, Filename, 0, KEY_READ, &HResult)) == ERROR_SUCCESS)
    {
        if ((ErrorValue = RegQueryInfoKey(HResult, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            char SubkeyName[_MAX_PATH] = { 0 };
            for (DWORD i = 0; i < count; i++)
            {
                if ((ErrorValue = RegEnumKey(HResult, i, SubkeyName, _MAX_PATH)) == ERROR_SUCCESS) //枚举所有值
                {
                    if ((ErrorValue = RegDeleteKey(HResult, SubkeyName)) != ERROR_SUCCESS)
                    {
                        ErrorCount++;
                        printf("\n error 该键 %s 存在子项 无法删除 \n", SubkeyName);
                    }
                }
                else
                {
                    ErrorCount++;
                    printf("\n error \n");
                }
            }
        }
        else
        {
            ErrorCount++;
            printf("\n error \n");
        }
        RegCloseKey(HResult);
        return true;
    }
    else
    {
        ErrorCount++;
        printf("\n error %s 不存在 or 需要管理员权限 \n", Filename);
    }
    if (ErrorCount == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DeleteRegisteredKey(HKEY HKey, const char* Filename, const char* SubkeyName)
{
    HKEY HResult;
    long value = -1;
    if ((value = RegOpenKeyEx(HKey, Filename, 0, KEY_READ, &HResult)) == ERROR_SUCCESS)
    {
        if ((value = RegDeleteKey(HResult, SubkeyName)) == ERROR_SUCCESS)
        {
            printf("\n error %s 不存在 or 需要管理员权限 \n", SubkeyName);
        }
        RegCloseKey(HResult);
        return true;
    }
    return false;
}

bool CreateButton(char const* ExePath, char const* IconPath, char const* ButtonName)
{
    /*路径创建*/
    char KeyBuf[_MAX_PATH] = { 0 };
    char SubkeyBuf[_MAX_PATH] = { 0 };

    strcpy_s(KeyBuf, _MAX_PATH, REG_PATH_TEST);
    strcat_s(KeyBuf, _MAX_PATH, ButtonName); //新建项 路径名

    strcat_s(SubkeyBuf, _MAX_PATH, KeyBuf);
    strcat_s(SubkeyBuf, _MAX_PATH, "\\command"); //command 路径名

    /*根注册表项构建*/
    FregInfo RegInfo;
    InitFregInfo(&RegInfo);
    strcpy_s(RegInfo.Filename, _MAX_PATH, KeyBuf);
    RegInfo.HKey = HKEY_CLASSES_ROOT;

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, ButtonName);

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount].ValueName, _MAX_PATH, "Icon");
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, IconPath);

    /*根项注册*/
    if (!RegisterInfo(&RegInfo))
    {
        printf("\n 1 需要管理员权限~~ \n");
        system("pause");

        return false;
    }

	/*子注册表项构建*/
    InitFregInfo(&RegInfo);
    strcpy_s(RegInfo.Filename, _MAX_PATH, SubkeyBuf);
    RegInfo.HKey = HKEY_CLASSES_ROOT;

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, ExePath);

	/*子项注册*/
    if (!RegisterInfo(&RegInfo))
    {
        printf("\n 2 需要管理员权限~~ \n");
        system("pause");

        return false;
    }
    return true;
}

bool DestroyButton(char const* ButtonName)
{
    char KeyBuf[_MAX_PATH] = { 0 };
    strcpy_s(KeyBuf, _MAX_PATH, REG_PATH_TEST);
    strcat_s(KeyBuf, _MAX_PATH, ButtonName); //新建项 路径名

    if (DeleteRegisteredInfo(HKEY_CLASSES_ROOT, KeyBuf))
    {
        DeleteRegisteredKey(HKEY_CLASSES_ROOT, REG_PATH_TEST, ButtonName);
    }
    return true;
}
_CRT_END_C_HEADER