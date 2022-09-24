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
    REG_OPTION_NON_VOLATILE �������������� REG_OPTION_VOLATILE ���������Զ�����
    KEY_CREATE_SUB_KEY ֧�ִ����Ӽ� KEY_ALL_ACCESS ���Է���
    ERROR_SUCCESS ����ɹ�
    */

    HKEY HResult; //�洢���صľ��
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
                if ((ErrorValue = RegEnumKey(HResult, i, SubkeyName, _MAX_PATH)) == ERROR_SUCCESS) //ö������ֵ
                {
                    if ((ErrorValue = RegDeleteKey(HResult, SubkeyName)) != ERROR_SUCCESS)
                    {
                        ErrorCount++;
                        printf("\n error �ü� %s �������� �޷�ɾ�� \n", SubkeyName);
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
        printf("\n error %s ������ or ��Ҫ����ԱȨ�� \n", Filename);
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
            printf("\n error %s ������ or ��Ҫ����ԱȨ�� \n", SubkeyName);
        }
        RegCloseKey(HResult);
        return true;
    }
    return false;
}

bool CreateButton(char const* ExePath, char const* IconPath, char const* ButtonName)
{
    /*·������*/
    char KeyBuf[_MAX_PATH] = { 0 };
    char SubkeyBuf[_MAX_PATH] = { 0 };

    strcpy_s(KeyBuf, _MAX_PATH, REG_PATH_TEST);
    strcat_s(KeyBuf, _MAX_PATH, ButtonName); //�½��� ·����

    strcat_s(SubkeyBuf, _MAX_PATH, KeyBuf);
    strcat_s(SubkeyBuf, _MAX_PATH, "\\command"); //command ·����

    /*��ע������*/
    FregInfo RegInfo;
    InitFregInfo(&RegInfo);
    strcpy_s(RegInfo.Filename, _MAX_PATH, KeyBuf);
    RegInfo.HKey = HKEY_CLASSES_ROOT;

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, ButtonName);

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount].ValueName, _MAX_PATH, "Icon");
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, IconPath);

    /*����ע��*/
    if (!RegisterInfo(&RegInfo))
    {
        printf("\n 1 ��Ҫ����ԱȨ��~~ \n");
        system("pause");

        return false;
    }

	/*��ע������*/
    InitFregInfo(&RegInfo);
    strcpy_s(RegInfo.Filename, _MAX_PATH, SubkeyBuf);
    RegInfo.HKey = HKEY_CLASSES_ROOT;

    RegInfo.ValueCollection[RegInfo.ValueCount].ValueType = REG_SZ;
    strcpy_s(RegInfo.ValueCollection[RegInfo.ValueCount++].Buf, _MAX_PATH, ExePath);

	/*����ע��*/
    if (!RegisterInfo(&RegInfo))
    {
        printf("\n 2 ��Ҫ����ԱȨ��~~ \n");
        system("pause");

        return false;
    }
    return true;
}

bool DestroyButton(char const* ButtonName)
{
    char KeyBuf[_MAX_PATH] = { 0 };
    strcpy_s(KeyBuf, _MAX_PATH, REG_PATH_TEST);
    strcat_s(KeyBuf, _MAX_PATH, ButtonName); //�½��� ·����

    if (DeleteRegisteredInfo(HKEY_CLASSES_ROOT, KeyBuf))
    {
        DeleteRegisteredKey(HKEY_CLASSES_ROOT, REG_PATH_TEST, ButtonName);
    }
    return true;
}
_CRT_END_C_HEADER