#define _CRT_SECURE_NO_WARNINGS

#include "ClientEngine.h"

#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/FileOperate/Public/FileOperate.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/Algorithm/Public/StringAlgorithm.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/Array/Public/CArrayString.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Guid/Public/Guid.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Windows/Command/Public//WindowsSettings.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Path/Public/Path.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/Log/Public/LogSystem.h"

#include "../../GitProtocol/Source/GitProtocol.h"


void GetProjectVersionListByCommits(GitProjectVersionList* const OutVersionList, GitCommits const* const InCommits)
{
	for (int i = 0; i < InCommits->Size; i++)
	{
		char GuidBuffer[_MAX_PATH] = { 0 };
		GUIDToString(GuidBuffer, _MAX_PATH, &InCommits->Commits[i].Guid);

		char PathBuffer[_MAX_PATH] = { 0 };
		const char* const p = GetGitLocalGitPath();
		GetFormatString(PathBuffer, "%s\\%s.lst", p, GuidBuffer);

		char DataBuffer[8192] = { 0 };
		GetFileBuf(PathBuffer, DataBuffer, 8192);
		StringToGitProjectVersion(&OutVersionList->ProjectVersionList[i], DataBuffer);
		OutVersionList->Size++;
	}
}

DWORD WINAPI GitClientSend(LPVOID lpParam)
{
	EngineLoop();
	return 0;
}

DWORD WINAPI GitClientReceive(LPVOID lpParam)
{
	while (!bExit)
	{
		if (_access("./GitClientCache/type.protocol", 0) == 0) {
			GitLock();
			unsigned char const Protocol = GetProtocolType();

			switch ((EGitProtocolType)Protocol)
			{
			case NONE:
			{
				break;
			}
			case HI:
			{
				ReceiveProtocol = Protocol;
				ResumeThread(hWorkThread);
				break;
			}
			case VERSION_LOG:
			case VERSION_STATUS:
			case VERSION_PROTOCOL:
			case VERSION_LIST:
			case VERSION_NEXT:
			case VERSION_NEXT_COMPLETE:
			case CLASH_CONTENT:
			case NO_CLASH:
			{
				Sleep(100);
				InitNetworkDataBufferAndProtocol();
				GetProtocolContent(NetworkDataBuffer);

				ReceiveProtocol = Protocol;
				ResumeThread(hWorkThread);
				break;
			}
			case SERVER_ERROR:
			{
				LOG_ERROR("Server Error");
				ResumeThread(hWorkThread);
				break;
			}
			}
			Sleep(500);
			GitUnlock();
		}
	}

	return 0;
}

unsigned char GetProtocolType(void)
{
	unsigned char ProtocolNumber = 0;

	char FileName[_MAX_PATH] = { 0 };
	strcpy_s(FileName, _MAX_PATH, GetGitLocalClientCachePath());
	strcat_s(FileName, _MAX_PATH, "\\type.protocol");

	if (_access(FileName, 0) == 0)
	{
		char ContentBuffer[_MAX_PATH] = { 0 };

		if (GetFileBuf(FileName, ContentBuffer, _MAX_PATH))
		{
			remove(FileName);
		}

		ProtocolNumber = (unsigned char)atoi(ContentBuffer);
	}

	return ProtocolNumber;
}

void GetProtocolContent(char* Buffer)
{
	char ContentFileName[_MAX_PATH];
	strcpy(ContentFileName, GitLocalClientCachePath);
	strcat(ContentFileName, "\\content.protocol");
	if (_access(ContentFileName, 0) == 0)
	{
		if (GetFileBuf(ContentFileName, Buffer, 8192))
		{
			remove(ContentFileName);
		}
	}
}

char const* GetGitExecutePath()
{
	if (GitLocalExecutePath[0] == '\0')
	{
		_getcwd(GitLocalExecutePath, _MAX_PATH);
	}
	return GitLocalExecutePath;
}

char const* GetGitLocalRepositoryPath(void)
{
	if (GitLocalRepositoryPath[0] == '\0')
	{
		bool bExist = false;
		char CurWorkPath[_MAX_PATH] = { 0 };
		CArrayString CString;
		InitCArrayString(&CString);
		DismantlingCArrayString(_getcwd(CurWorkPath, _MAX_PATH), "\\", &CString);

		for (int i = 0; i < CString.size; i++)
		{
			char* p = GetCArrayString(i, &CString);
			strcat_s(GitLocalRepositoryPath, _MAX_PATH, p);

			char BufTmp[_MAX_PATH] = { 0 };
			strcpy_s(BufTmp, _MAX_PATH, GitLocalRepositoryPath);
			strcat_s(BufTmp, _MAX_PATH, "\\.gitX");
			if (_access(BufTmp, 0) == 0)
			{
				bExist = true;
				break;
			}

			strcat_s(GitLocalRepositoryPath, _MAX_PATH, "\\");
		}

		DestroyCArrayString(&CString);

		if (!bExist)
		{
			memset(GitLocalRepositoryPath, 0, _MAX_PATH);
		}
	}
	return GitLocalRepositoryPath;
}

char const* GetGitLocalClientCachePath(void)
{
	if (GitLocalClientCachePath[0] == '\0')
	{
		strcat(_getcwd(GitLocalClientCachePath, _MAX_PATH),  "\\GitClientCache");
	}
	return GitLocalClientCachePath;
}

char const* GetGitLocalConfigFileName(void)
{
	if (GitLocalConfigFileName[0] == '\0')
	{
		if (GitLocalConfigPath[0] == '\0')
		{
			strcat(_getcwd(GitLocalConfigPath, _MAX_PATH), "\\Config");
		}
		strcpy(GitLocalConfigFileName, GitLocalConfigPath);
		strcat(GitLocalConfigFileName, "\\Config.ini");
	}
	return GitLocalConfigFileName;
}

char const* GetGitLocalUserFileName(void)
{
	if (GitLocalUserFileName[0] == '\0')
	{
		if (GitLocalUserPath[0] == '\0')
		{
			strcat(_getcwd(GitLocalUserPath, _MAX_PATH), "\\User");
		}
		strcpy(GitLocalUserFileName, GitLocalUserPath);
		strcat(GitLocalUserFileName, "\\User.ini");
	}
	return GitLocalUserFileName;
}

char const* GetGitRemoteRepositoryPath(void)
{
	return GitRemoteRepositoryPath;
}

void ClearCache(void)
{
	char ContentPath[_MAX_PATH] = { 0 };
	char ProtocolTypePath[_MAX_PATH] = { 0 };

	strcpy(ContentPath, GitLocalClientCachePath);
	strcpy(ProtocolTypePath, GitLocalClientCachePath);
	strcat(ProtocolTypePath, "\\type.protocol");
	strcat(ContentPath, "\\content.protocol");
	remove(ContentPath);
	remove(ProtocolTypePath);
}

void VersionIterativeUpdate()
{
	GitIsServerExistType(GitRemoteOrigin);
	GitIsServerExistContent(GitRemoteOrigin);

	EGitProtocolType Type = NONE;
	char ReceiveBuffer[8192] = { 0 };
	// 1.0 检查当前版本是否最新，获取服务器最新版本号，然后比较 看看是不是一样的。
	if (GitSendProtocolType(GitRemoteOrigin, VERSION_PROTOCOL, SERVER_TYPE_URL) && 
		GitSendContent(GitRemoteOrigin, GetGitExecutePath(), SERVER_CONTENT_URL))
	{
		GitIsServerExistType(GitRemoteOrigin);
		GitIsServerExistContent(GitRemoteOrigin);
		LOG_SUCCESS("Get server newest version number!");
		GitReceive(&Type, ReceiveBuffer);

		GitCommit ServerLatestCommit;
		InitGitCommit(&ServerLatestCommit);
		StringToGitCommit(&ServerLatestCommit, ReceiveBuffer);

		GitCommit* ClientLatestCommit = &Commits.Commits[Commits.Size - 1];

		if (!GuidEqual(&ServerLatestCommit.Guid, &ClientLatestCommit->Guid))
		{
			//验证冲突
			char ContentString[8192] = { 0 };

			GUIDToString(ContentString, 8192, &ClientLatestCommit->Guid);
			strcat(ContentString, "|");
			strcat(ContentString, GetGitExecutePath());
			strcat(ContentString, "|");

			for (int i = 0; i < ProjectVersion.Size; i++)
			{
				RemoveStringStart(ProjectVersion.FileVersionList[i].FileName, GitLocalRepositoryPath);
				strcat(ContentString, ProjectVersion.FileVersionList[i].FileName);
				strcat(ContentString, "\n");
				char GuidBuffer[_MAX_PATH] = { 0 };
				GUIDToString(GuidBuffer, _MAX_PATH, &ProjectVersion.FileVersionList[i].CRC);
				strcat(ContentString, GuidBuffer);
				strcat(ContentString, "\n");
				char SizeBuffer[_MAX_PATH] = { 0 };
				strcat(ContentString, _itoa((int)ProjectVersion.FileVersionList[i].FileSize, SizeBuffer, 10));
				strcat(ContentString, "\n");
			}
			RemoveCharEnd(ContentString, '\n');

			if (GitSendProtocolType(GitRemoteOrigin, VERSION_CLASH, SERVER_TYPE_URL) &&
				GitSendContent(GitRemoteOrigin, ContentString, SERVER_CONTENT_URL))
			{
				GitIsServerExistType(GitRemoteOrigin);
				GitIsServerExistType(GitRemoteOrigin);
				Type = NONE;
				memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
				GitReceive(&Type, ReceiveBuffer);

				if(Type == CLASH_CONTENT)
				{
					ReplaceCharAll(ReceiveBuffer, '\n', ' ');
					LOG_ERROR("%s", ReceiveBuffer);
					LOG_ERROR("Please handle clash first");
				}
				else if(Type == NO_CLASH)
				{
					memset(ContentString, 0, sizeof(ContentString));
					GUIDToString(ContentString, 8192, &ClientLatestCommit->Guid);

					strcat(ContentString, "\n");
					strcat(ContentString, GetGitExecutePath());
					strcat(ContentString, "\n");
					strcat(ContentString, GetGitLocalRepositoryPath());

					// 1.1 把当前客户端版本发送到服务器
					if (GitSendProtocolType(GitRemoteOrigin, VERSION_NEXT, SERVER_TYPE_URL) &&
						GitSendContent(GitRemoteOrigin, ContentString, SERVER_CONTENT_URL))
					{
						GitIsServerExistType(GitRemoteOrigin);
						GitIsServerExistContent(GitRemoteOrigin);
						// 1.2 接受新版本号 存储在本地，替换
						LOG_SUCCESS("Need to update next version!");
						Type = NONE;
						memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
						GitReceive(&Type, ReceiveBuffer);

						if (Type == VERSION_NEXT)
						{
							StringToGitCommit(&Commits.Commits[Commits.Size++], ReceiveBuffer);
							AddFileBuf(GetGitProjectVersionListFileName(), ReceiveBuffer);

							Type = NONE;
							memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
							GitReceive(&Type, ReceiveBuffer);

							if (Type == VERSION_NEXT_COMPLETE)
							{
								AddNewFileBuf(GetGitProjectVersionFileName(), ReceiveBuffer);
								VersionIterativeUpdate();
							}
						}
					}
				}
			}
		}
		else
		{
			LOG_GENERAL("Current version is the newest version");
		}

	}
}

void CompareList(GitProjectVersion const* const ServerVersion, PathCollection const* const LocalPaths, PathCollection* const OutPaths)
{
	const char* ClientUrl = GetGitLocalRepositoryPath();
	char* bDelete = malloc(ServerVersion->Size * sizeof(char));
	memset(bDelete, 1, ServerVersion->Size * sizeof(char));

	for (int i = 0; i < LocalPaths->index; i++)
	{
		AddProjectVersion(&ProjectVersion, LocalPaths->paths[i]);

		char TmpPath[_MAX_PATH] = { 0 };
		strcpy(TmpPath, LocalPaths->paths[i]);
		RemoveStringStart(TmpPath, ClientUrl);

		bool bExist = false;
		for (int j = 0; j < ServerVersion->Size; j++)
		{
			if (strcmp(ServerVersion->FileVersionList[j].FileName, TmpPath) == 0)
			{
				bExist = true;
				bDelete[j] = 0;
				unsigned Size = GetFileSizeByFileName(LocalPaths->paths[i]);
				if (Size != ServerVersion->FileVersionList[j].FileSize)
				{
					strcpy(OutPaths->paths[OutPaths->index++], LocalPaths->paths[i]);
				}
				break;
			}
		}
		if(!bExist)
		{
			strcpy(OutPaths->paths[OutPaths->index++], LocalPaths->paths[i]);
		}
	}
	for (unsigned i = 0; i < ServerVersion->Size; i++)
	{
		if (bDelete[i] == 1)
		{
			strcat(RemovedFileBuffer, ServerVersion->FileVersionList[i].FileName);
			strcat(RemovedFileBuffer, "\n");
		}
	}
	free(bDelete);
}

void GitAddLocalData(GitProjectVersion const* const ServerVersion, char const* TargetFile)
{
	//清空上一版本
	InitProjectVersion(&ProjectVersion);

	char FilePathBuffer[_MAX_PATH] = { 0 };
	strcpy(FilePathBuffer, GitLocalRepositoryPath);

	strcat(FilePathBuffer, "\\");
	if(!strstr(TargetFile, "."))
	{
		strcat(FilePathBuffer, TargetFile);
	}
	
	PathCollection OldFilePathCollection;
	InitPathCollection(&OldFilePathCollection);
	FindFiles(FilePathBuffer, &OldFilePathCollection, true);

	PathCollection FinalFilePathCollection;
	InitPathCollection(&FinalFilePathCollection);
	CompareList(ServerVersion, &OldFilePathCollection, &FinalFilePathCollection);

	/*//获取待拷贝项目名
	CArrayString CFile;
	InitCArrayString(&CFile);
	if (strstr(GitLocalRepositoryPath, "\\"))
	{
		DismantlingCArrayString(GitLocalRepositoryPath, "\\", &CFile);
	}
	else if (strstr(GitLocalRepositoryPath, "/"))
	{
		DismantlingCArrayString(GitLocalRepositoryPath, "/", &CFile);
	}
	else
	{
		LOG_ERROR("Invalid Path: %s", GitLocalRepositoryPath);
		break;
	}
	char* ProjectName = GetCArrayString(CFile.size - 1, &CFile);
	RemoveCharEnd(ProjectName, strlen(ProjectName), '\n');*/

	//组装远端项目路径
	char RemotePathBuf[_MAX_PATH] = { 0 };
	GetFormatString(RemotePathBuf, "%s", GetGitRemoteRepositoryPath());

	//复制文件
	for (int i = 0; i < FinalFilePathCollection.index; i++)
	{
		//组装远端文件路径名
		//获取本地文件相对路径名
		char RemoteFilePathRelative[_MAX_PATH];
		strcpy_s(RemoteFilePathRelative, _MAX_PATH, FinalFilePathCollection.paths[i]);
		RemoveStringStart(RemoteFilePathRelative, GitLocalRepositoryPath);

		//组装远端文件绝对路径名
		char RemoteFilePathAbsolute[_MAX_PATH] = { 0 };
		strcpy_s(RemoteFilePathAbsolute, _MAX_PATH, RemotePathBuf);
		strcat_s(RemoteFilePathAbsolute, sizeof(RemoteFilePathAbsolute), RemoteFilePathRelative);

		//创建远端项目目录
		char RemoteDocumentPath[_MAX_PATH] = { 0 };
		GetPathDirectory(RemoteDocumentPath, _MAX_PATH, RemoteFilePathAbsolute);

		if (!CreateFileDirectory(RemoteDocumentPath))
		{
			LOG_ERROR("Generate a conflict : %s", RemoteDocumentPath);
			continue;
		}

		ToSendFiles.Size++;
		strcpy_s(ToSendFiles.Paths[i].SrcPath, _MAX_PATH, FinalFilePathCollection.paths[i]);
		strcpy_s(ToSendFiles.Paths[i].DestPath, _MAX_PATH, RemoteFilePathAbsolute);

		double Progress = ((double)i + 1.0) / (double)ToSendFiles.Size * 100;
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("%4.1f%%", Progress);
	}
	/*DestroyCArrayString(&CFile);*/
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	printf("%4.1f%% \r\n", 100.0);
}

void ShowCommandLine()
{
	SetWindowFontRepresentColor(GREEN, 0);
	printf("%s ssh : ", NewUser.Name);

	SetWindowFontRepresentColor(LAVENDER, 0);
	printf("Windows64 ");

	SetWindowFontRepresentColor(CANARY_YELLOW, 0);
	printf(" %s \r\n", GitCurWorkPath);

	SetWindowFontRepresentColor(WHITE, 0);
	printf("$ ");
}

void CreateLocalFile()
{
	if (_access(".\\GitClientCache", 0) != 0)
	{
		_mkdir(".\\GitClientCache");
	}
	GetGitLocalClientCachePath();

	if (_access(".\\Config\\Config.ini", 0) != 0)
	{
		if (_access(".\\Config", 0) != 0)
		{
			_mkdir(".\\Config");
		}
		CCreateFile(".\\Config\\Config.ini");
		SaveConfig();
	}
	GetGitLocalConfigFileName();

	if (_access(".\\User\\User.ini", 0) != 0)
	{
		if (_access(".\\User", 0) != 0)
		{
			_mkdir(".\\User");
		}
		CCreateFile(".\\User\\User.ini");
		SaveUser();
	}
	GetGitLocalUserFileName();
}

void SaveConfig()
{
	char const* ConfigPathBuffer = GetGitLocalConfigFileName();

	FILE* FilePtr = NULL;

	if (fopen_s(&FilePtr, ConfigPathBuffer, "w+") == 0)
	{
		fprintf(FilePtr, "RepositoryPath=\"%s\"\n", GetGitLocalRepositoryPath());
		fprintf(FilePtr, "LastWorkPath=\"%s\"", GetGitCurWorkPath());
		fclose(FilePtr);
	}
}

void ReadConfig()
{
	char const* ConfigPathBuffer = GetGitLocalConfigFileName();

	char ConfigContentBuffer[2048] = { 0 };
	if (GetFileBuf(ConfigPathBuffer, ConfigContentBuffer, 2048))
	{
		CArrayString StrSentence;
		InitCArrayString(&StrSentence);

		DismantlingCArrayString(ConfigContentBuffer, "\n", &StrSentence);
		for (int i = 0; i < StrSentence.size; i++)
		{
			char* Tmp = GetCArrayString(i, &StrSentence);
			if (strstr(Tmp, "RepositoryPath="))
			{
				DismantlingStringByIndex(1, Tmp, "=", GitLocalRepositoryPath, _MAX_PATH);
				RemoveCharEnd(GitLocalRepositoryPath, '\"');
				RemoveCharStart(GitLocalRepositoryPath, '\"');
			}
			else if (strstr(Tmp, "LastWorkPath="))
			{
				DismantlingStringByIndex(1, Tmp, "=", GitCurWorkPath, _MAX_PATH);
				RemoveCharEnd(GitCurWorkPath, '\"');
				RemoveCharStart(GitCurWorkPath, '\"');
			}
		}
		DestroyCArrayString(&StrSentence);
	}
}

void GetGitCommit(GitCommit* OutCommit)
{
	
	char CommitBuffer[1024] = { 0 };
	GetFileBuf(GetGitProjectVersionListFileName(), CommitBuffer, 1024);
	InitGitCommit(OutCommit);
	StringToGitCommit(OutCommit, CommitBuffer);

}

char* GitCommitToString(char* const OutBuffer, GitCommit const* const InCommit)
{
	char GuidBuffer[_MAX_PATH] = { 0 };
	GUIDToString(GuidBuffer, _MAX_PATH, &InCommit->Guid);
	GetFormatStringS(OutBuffer, 512, "%s\n%s\n%s\n%s\n", InCommit->Name, InCommit->Commit, InCommit->Date, GuidBuffer);

	return OutBuffer;
}

void StringToGitCommit(GitCommit* const OutCommit, char const* const InBuffer)
{
	InitGitCommit(OutCommit);

	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(InBuffer, "\n", &CString);
	{
		char* value = GetCArrayString(0, &CString);
		strcpy(OutCommit->Name, value);
	}
	{
		char* value = GetCArrayString(1, &CString);
		strcpy(OutCommit->Commit, value);
	}
	{
		char* value = GetCArrayString(2, &CString);
		strcpy(OutCommit->Date, value);
	}
	{
		char* value = GetCArrayString(3, &CString);
		StringToGUID(&OutCommit->Guid, value);
	}
	DestroyCArrayString(&CString);
}

void StringToGitCommits(GitCommits* const OutCommits, char const* const InBuffer)
{
	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(InBuffer, "\n", &CString);

	for (int i = 0; i < CString.size; i++)
	{
		char* p = GetCArrayString(i++, &CString);
		strcpy(OutCommits->Commits[OutCommits->Size].Name, p);

		p = GetCArrayString(i++, &CString);
		strcpy(OutCommits->Commits[OutCommits->Size].Commit, p);

		p = GetCArrayString(i++, &CString);
		strcpy(OutCommits->Commits[OutCommits->Size].Date, p);

		p = GetCArrayString(i, &CString);
		StringToGUID(&OutCommits->Commits[OutCommits->Size++].Guid, p);
	}
	DestroyCArrayString(&CString);
}

void InitGitCommit(GitCommit* OutCommit)
{
	memset(OutCommit->Name, 0, _MAX_PATH);
	memset(OutCommit->Commit, 0, _MAX_PATH);
	memset(OutCommit->Date, 0, TIMEBUF_SIZE);
	InitGUID(&OutCommit->Guid);
}

void InitGitCommits(GitCommits* OutCommits)
{
	OutCommits->Size = 0;
	memset(OutCommits->Commits, 0, sizeof(OutCommits->Commits));
}

void InitGitPath2Ds(GitPath2Ds* Path2Ds)
{
	Path2Ds->Size = 0;
	memset(Path2Ds->Paths, 0, sizeof(Path2Ds->Paths));
}

char const* GetGitLocalGitPath(void)
{
	if (GitLocalGitPath[0] == '\0')
	{
		strcpy(GitLocalGitPath, GitLocalRepositoryPath);
		strcat(GitLocalGitPath, "\\.gitX");
	}
	return GitLocalGitPath;
}

void InitProjectVersion(GitProjectVersion* FileVersions)
{
	FileVersions->Size = 0;
	memset(FileVersions->FileVersionList, 0, sizeof(FileVersions->FileVersionList));
}

void AddProjectVersion(GitProjectVersion* TargetProjectVersion, char const* InPath)
{
	//获取相对路径
	char NewPath[_MAX_PATH] = { 0 };
	strcpy(NewPath, InPath);
	RemoveStringStart(NewPath, GetGitLocalRepositoryPath());

	strcpy(TargetProjectVersion->FileVersionList[TargetProjectVersion->Size].FileName, NewPath);
	CreateGUID(&TargetProjectVersion->FileVersionList[TargetProjectVersion->Size].CRC);
	TargetProjectVersion->FileVersionList[TargetProjectVersion->Size].FileSize = GetFileSizeByFileName(InPath);

	TargetProjectVersion->Size++;
}

char const* GetGitProjectVersionListFileName(void)
{
	if (GitProjectVersionListFileName[0] == '\0' && GitLocalGitPath[0] != '\0')
	{
		strcpy(GitProjectVersionListFileName, GitLocalGitPath);
		strcat(GitProjectVersionListFileName, "\\version.list");
	}
	return GitProjectVersionListFileName;
}

char const* GetGitProjectVersionFileName(void)
{
	memset(GitCurAllFileVersionFileName, 0, sizeof(GitCurAllFileVersionFileName));
	strcpy(GitCurAllFileVersionFileName, GitLocalGitPath);
	strcat(GitCurAllFileVersionFileName, "\\");

	GitCommit const* LastCommit = &Commits.Commits[Commits.Size - 1];

	if (LastCommit)
	{
		char GuidBuffer[_MAX_PATH] = { 0 };
		GUIDToString(GuidBuffer, _MAX_PATH, &LastCommit->Guid);
		strcat(GitCurAllFileVersionFileName, GuidBuffer);
		strcat(GitCurAllFileVersionFileName, ".lst");
	}

	return GitCurAllFileVersionFileName;
}

char const* GetGitLocalUserPath(void)
{
	_getcwd(GitLocalUserPath, _MAX_PATH);
	strcat(GitLocalUserPath, "\\user");

	return GitLocalUserPath;
}

char const* GetGitCurWorkPath()
{
	if (GitCurWorkPath[0] == '\0')
	{
		_getcwd(GitCurWorkPath, _MAX_PATH);
	}
	return GitCurWorkPath;
}

void GitFileVersionsToString(char* const OutBuffer, GitProjectVersion const* const InFileVersions)
{
	for(unsigned i = 0; i < InFileVersions->Size; i++)
	{
		char TmpBuffer[_MAX_PATH * 2] = { 0 };
		char GuidBuffer[_MAX_PATH] = { 0 };

		GUIDToString(GuidBuffer, _MAX_PATH, &InFileVersions->FileVersionList[i].CRC);
		GetFormatStringS(TmpBuffer, _MAX_PATH * 2, "%s\n%s\n%u\n",
			InFileVersions->FileVersionList[i].FileName,
			GuidBuffer,
			InFileVersions->FileVersionList[i].FileSize
		);

		strcat(OutBuffer, TmpBuffer);
	}
}

void StringToGitProjectVersion(GitProjectVersion* const OutFileVersions, char const* const InBuffer)
{
	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(InBuffer, "\n", &CString);

	for(int i = 0; i < CString.size; i++)
	{
		char* p = GetCArrayString(i++, &CString);
		strcpy(OutFileVersions->FileVersionList[OutFileVersions->Size].FileName, p);

		p = GetCArrayString(i++, &CString);
		StringToGUID(&OutFileVersions->FileVersionList[OutFileVersions->Size].CRC, p);

		p = GetCArrayString(i, &CString);
		OutFileVersions->FileVersionList[OutFileVersions->Size++].FileSize = (unsigned)atoi(p);
	}
	DestroyCArrayString(&CString);
}

void ReadUser(void)
{
	char const* UserPathBuffer = GetGitLocalUserFileName();

	char UserContentBuffer[2048] = { 0 };
	if(GetFileBuf(UserPathBuffer, UserContentBuffer, 2048))
	{
		CArrayString StrSentence;
		InitCArrayString(&StrSentence);

		DismantlingCArrayString(UserContentBuffer, "\n", &StrSentence);
		for (int i = 0; i < StrSentence.size; i++)
		{
			char* Tmp = GetCArrayString(i, &StrSentence);
			if (strstr(Tmp, "Name="))
			{
				DismantlingStringByIndex(1, Tmp, "=", NewUser.Name, _MAX_PATH);
				RemoveCharEnd(NewUser.Name, '\"');
				RemoveCharStart(NewUser.Name, '\"');
			}
			else if (strstr(Tmp, "Password="))
			{
				DismantlingStringByIndex(1, Tmp, "=", NewUser.Password, _MAX_PATH);
				RemoveCharEnd(NewUser.Password, '\"');
				RemoveCharStart(NewUser.Password, '\"');
			}
			else if (strstr(Tmp, "Mail="))
			{
				DismantlingStringByIndex(1, Tmp, "=", NewUser.Mail, _MAX_PATH);
				RemoveCharEnd(NewUser.Mail, '\"');
				RemoveCharStart(NewUser.Mail, '\"');
			}
			else if (strstr(Tmp, "RemoteOrigin="))
			{
				DismantlingStringByIndex(1, Tmp, "=", GitRemoteOrigin, _MAX_PATH);
				RemoveCharEnd(GitRemoteOrigin, '\"');
				RemoveCharStart(GitRemoteOrigin, '\"');
			}
			else if (strstr(Tmp, "RemoteRepository="))
			{
				DismantlingStringByIndex(1, Tmp, "=", GitRemoteRepositoryPath, _MAX_PATH);
				RemoveCharEnd(GitRemoteRepositoryPath, '\"');
				RemoveCharStart(GitRemoteRepositoryPath, '\"');
			}
		}
		DestroyCArrayString(&StrSentence);
	}
}

void SaveUser(void)
{
	char const* UserPathBuffer = GetGitLocalUserFileName();

	FILE* FilePtr = NULL;

	if(fopen_s(&FilePtr, UserPathBuffer, "w+") == 0)
	{
		fprintf_s(FilePtr, "Name=\"%s\"\n", NewUser.Name);
		fprintf_s(FilePtr, "Password=\"%s\"\n", NewUser.Password);
		fprintf_s(FilePtr, "Mail=\"%s\"\n", NewUser.Mail);
		fprintf_s(FilePtr, "RemoteOrigin=\"%s\"\n", GitRemoteOrigin);
		fprintf_s(FilePtr, "RemoteRepository=\"%s\"", GitRemoteRepositoryPath);
		fclose(FilePtr);
	}
}

void EngineInit(void)
{
	//如果配置文件不存在则创建配置文件目录结构
	CreateLocalFile();

	//读取配置信息
	ReadConfig();
	ReadUser();
	if (*(GetGitLocalRepositoryPath()) != '\0')
	{
		GetGitLocalGitPath();
		GetGitProjectVersionListFileName();
		char RepositoryLogPath[_MAX_PATH] = { 0 };
		strcpy(RepositoryLogPath, GetGitLocalGitPath());
		strcat(RepositoryLogPath, "\\logs");
		InitLogSystem(RepositoryLogPath);

		char CommitsBuffer[8192] = { 0 };
		GetFileBuf(GitProjectVersionListFileName, CommitsBuffer, 8192);
		StringToGitCommits(&Commits, CommitsBuffer);
		GetProjectVersionListByCommits(&ProjectVersionList, &Commits);
	}
	else
	{
		InitGitCommits(&Commits);
	}
	
	//清空缓存
	ClearCache();

	//初始化全局变量
	GitInitMutex();
	InitNetworkDataBufferAndProtocol();

	hWorkThread = CreateThread(NULL, 100000000, GitClientSend, NULL, 0, NULL);
	hReceiveThread = CreateThread(NULL, 100000000, GitClientReceive, NULL, 0, NULL);
}

void EngineLoop(void)
{
	char InputBuf[1024] = { 0 };

	while (!bExit)
	{
		ShowCommandLine();

		fgets(InputBuf, 1024, stdin);
		if (strstr(InputBuf, "exit"))
		{
			bExit = true;
		}
		else if (strstr(InputBuf, "cd"))
		{
			CArrayString CString;
			InitCArrayString(&CString);
			DismantlingCArrayString(InputBuf, " ", &CString);
			RemoveCharEnd(InputBuf, '\n');

			strcpy(GitCurWorkPath, GetCArrayString(1, &CString));
			SaveConfig();

			DestroyCArrayString(&CString);
		}
		else if (strstr(InputBuf, "git init"))
		{
			strcpy(GitLocalRepositoryPath, GitCurWorkPath);

			char TmpPath[_MAX_PATH] = { 0 };
			strcpy(TmpPath, GetGitLocalRepositoryPath());
			strcat(TmpPath, "\\.gitX");

			if (_mkdir(TmpPath) == 0)
			{
				strcpy(GitLocalGitPath, TmpPath);
				strcat(TmpPath, "\\git.txt");

				char RepositoryLogPath[_MAX_PATH] = { 0 };
				strcpy(RepositoryLogPath, GetGitLocalGitPath());
				strcat(RepositoryLogPath, "\\logs");
				InitLogSystem(RepositoryLogPath);

				if (CCreateFile(TmpPath))
				{
					if (CCreateFile(GetGitProjectVersionListFileName()))
					{
						LOG_SUCCESS("git init success! %s", GetGitLocalRepositoryPath());
					}
					else
					{
						LOG_ERROR("git init failure!");
					}
				}
				else
				{
					LOG_ERROR("git init failure!");
				}
			}
			SaveConfig();
		}
		else if (strstr(InputBuf, "git status"))
		{
			GitIsServerExistType(GitRemoteOrigin);
			GitIsServerExistContent(GitRemoteOrigin);
			if(GitSendProtocolType(GitRemoteOrigin, VERSION_STATUS, SERVER_TYPE_URL))
			{
				GitCommit LastCommit;
				GetGitCommit(&LastCommit);

				char GuidBuffer[_MAX_PATH];
				GUIDToString(GuidBuffer, _MAX_PATH, &LastCommit.Guid);

				char ContentBuffer[8192] = { 0 };
				strcpy(ContentBuffer, GetGitExecutePath());
				strcat(ContentBuffer, "\n");
				strcat(ContentBuffer, GuidBuffer);

				if(GitSendContent(GitRemoteOrigin, ContentBuffer, SERVER_CONTENT_URL))
				{
					LOG_SUCCESS("Address & Guid send successed!");

					EGitProtocolType Type = NONE;
					char ReceiveBuffer[8196 * 1024] = { 0 };
					GitReceive(&Type, ReceiveBuffer);
					if(Type == VERSION_STATUS)
					{
						if(ReceiveBuffer[0] != '\0')
						{
							GitProjectVersion ReceiveFileVersions;
							InitProjectVersion((&ReceiveFileVersions));

							StringToGitProjectVersion(&ReceiveFileVersions, ReceiveBuffer);
							SetWindowFontRepresentColor(RED, 0);
							for(int i = 0; i < ReceiveFileVersions.Size; i++)
							{
								printf("\n Current version: %s\n", ReceiveFileVersions.FileVersionList[i].FileName);
							}
							SetWindowFontRepresentColor(WHITE, 0);
						}
					}
				}
			}
		}
		else if (strstr(InputBuf, "git log"))
		{
			GitIsServerExistType(GitRemoteOrigin);
			GitIsServerExistContent(GitRemoteOrigin);
			if(GitSendProtocolType(GitRemoteOrigin, VERSION_LOG, SERVER_TYPE_URL))
			{
				if(GitSendContent(GitRemoteOrigin, GetGitExecutePath(), SERVER_CONTENT_URL))
				{
					LOG_SUCCESS("Client Address successfully send!");

					EGitProtocolType Type = NONE;
					char LogBuffer[8192] = { 0 };
					GitReceive(&Type, LogBuffer);
					if(Type == VERSION_LOG && LogBuffer != '\0')
					{
						GitCommits Commits;
						InitGitCommits(&Commits);

						StringToGitCommits(&Commits, LogBuffer);
						for(int i = 0; i < Commits.Size; i++)
						{
							char GuidBuffer[_MAX_PATH] = { 0 };
							GUIDToString(GuidBuffer, _MAX_PATH, &Commits.Commits[i].Guid);
							SetWindowFontRepresentColor(YELLOW, 0);
							printf("\nVersion    : %s \r\n", GuidBuffer);
							SetWindowFontRepresentColor(WHITE, 0);
							printf("\nPeople     : %s \r\n", Commits.Commits[i].Name);
							printf("\nInformation: %s \r\n", Commits.Commits[i].Commit);
							printf("\nDate       : %s \r\n", Commits.Commits[i].Date);
						}
					}
					else
					{
						LOG_ERROR("Received but no useful information");
					}
				}
				else
				{
					LOG_ERROR("Client receive failed!");
				}
			}
		}
		else if (strstr(InputBuf, "git remote add origin"))
		{
			DismantlingStringByIndex(4, InputBuf, " ", GitRemoteOrigin, _MAX_PATH);
			DismantlingStringByIndex(5, InputBuf, " ", GitRemoteRepositoryPath, _MAX_PATH);

			LOG_SUCCESS("remote path set: %s", GitRemoteOrigin);

			SaveUser();
		}
		else if (strstr(InputBuf, "git --global user.name"))
		{
			DismantlingStringByIndex(3, InputBuf, " ", NewUser.Name, _MAX_PATH);

			LOG_SUCCESS("Set User Name Success : %s", NewUser.Name);

			SaveUser();
		}
		else if (strstr(InputBuf, "git --global user.password"))
		{
			DismantlingStringByIndex(3, InputBuf, " ", NewUser.Password, _MAX_PATH);

			LOG_SUCCESS("Set User Password Success : %s", NewUser.Password);

			SaveUser();
		}
		else if (strstr(InputBuf, "git --global user.email"))
		{
			DismantlingStringByIndex(3, InputBuf, " ", NewUser.Mail, _MAX_PATH);

			LOG_SUCCESS("Set Email Address Success : %s", NewUser.Mail);

			SaveUser();
		}
		else if (strstr(InputBuf, "git add"))
		{
			RemoveCharEnd(InputBuf, '\n');

			GitIsServerExistType(GitRemoteOrigin);
			GitIsServerExistContent(GitRemoteOrigin);

			EGitProtocolType Type = NONE;
			char ReceiveBuffer[8192 * 1024] = { 0 };
			// 检查当前版本是否最新，获取服务器最新版本号，然后比较 看看是不是一样的。
			if (GitSendProtocolType(GitRemoteOrigin, VERSION_PROTOCOL, SERVER_TYPE_URL) &&
				GitSendContent(GitRemoteOrigin, GetGitExecutePath(), SERVER_CONTENT_URL))
			{

				LOG_SUCCESS("Get server newest version number!");
				GitReceive(&Type, ReceiveBuffer);

				if(ReceiveBuffer[0] != '\0')
				{
					GitCommit ServerLatestCommit;
					InitGitCommit(&ServerLatestCommit);
					StringToGitCommit(&ServerLatestCommit, ReceiveBuffer);

					GitCommit* ClientLatestCommit = &Commits.Commits[Commits.Size - 1];

					if (GuidEqual(&ServerLatestCommit.Guid, &ClientLatestCommit->Guid))
					{
						GitIsServerExistType(GitRemoteOrigin);
						GitIsServerExistContent(GitRemoteOrigin);


						if (GitSendProtocolType(GitRemoteOrigin, VERSION_LIST, SERVER_TYPE_URL) &&
							GitSendContent(GitRemoteOrigin, GetGitExecutePath(), SERVER_CONTENT_URL))
						{
							Type = NONE;
							memset(ReceiveBuffer, 0, 8192 * 1024);
							GitReceive(&Type, ReceiveBuffer);

							if (Type == VERSION_LIST)
							{
								GitProjectVersion ServerVersion;
								InitProjectVersion(&ServerVersion);
								StringToGitProjectVersion(&ServerVersion, ReceiveBuffer);

								if (GitRemoteOrigin[0] != '\0')
								{
									InitGitPath2Ds(&ToSendFiles);
									//命令切割
									CArrayString CString;
									InitCArrayString(&CString);
									DismantlingCArrayString(InputBuf, " ", &CString);
									char const* AddTarget = GetCArrayString(2, &CString);

									GitAddLocalData(&ServerVersion, AddTarget);

								}
								else
								{
									LOG_ERROR("Invalid Path : %s", GitRemoteOrigin);
								}
							}
							else
							{
								LOG_ERROR("Protocol Failed!");
							}
						}

					}
					else
					{
						LOG_ERROR("Please use \"git pull\" to get newest version first!");
					}
				}
			}
		}
		else if (strstr(InputBuf, "git clone")) //注意：无法拷贝空文件夹
		{
			//命令切割
			CArrayString CString;
			InitCArrayString(&CString);
			DismantlingCArrayString(InputBuf, " ", &CString);

			//获取远端项目路径
			char* RemotePath = GetCArrayString(2, &CString);
			RemoveCharEnd(RemotePath, '\n');

			//获取待拷贝项目名
			CArrayString CFile;
			InitCArrayString(&CFile);
			if (strstr(RemotePath, "\\"))
			{
				DismantlingCArrayString(RemotePath, "\\", &CFile);
			}
			else if (strstr(RemotePath, "/"))
			{
				DismantlingCArrayString(RemotePath, "/", &CFile);
			}
			else
			{
				LOG_ERROR("Invalid Path: %s", RemotePath);
				break;
			}
			char* FileValue = GetCArrayString(CFile.size - 1, &CFile);
			RemoveCharEnd(FileValue, '\n');

			//组装本地项目路径
			char LocalPathBuf[_MAX_PATH] = { 0 };
			GetFormatString(LocalPathBuf,"%s\\%s", GitLocalRepositoryPath, FileValue);

			//获取远端待拷贝文件路径
			PathCollection OldFilePathCollection;
			InitPathCollection(&OldFilePathCollection);
			FindFiles(RemotePath, &OldFilePathCollection, true);

			//复制文件
			for(int i = 0; i < OldFilePathCollection.index; i++)
			{
				//组装本地文件路径名
				//获取本地文件相对路径名
				char LocalFilePathRelative[_MAX_PATH];
				strcpy_s(LocalFilePathRelative, _MAX_PATH, OldFilePathCollection.paths[i]);
				RemoveStringStart(LocalFilePathRelative, RemotePath);

				//组装本地文件绝对路径名
				char LocalFilePathAbsolute[_MAX_PATH] = { 0 };
				strcpy_s(LocalFilePathAbsolute, _MAX_PATH, LocalPathBuf);
				strcat_s(LocalFilePathAbsolute, sizeof(LocalFilePathAbsolute), LocalFilePathRelative);

				//创建本地项目目录
				char LocalDocumenPath[_MAX_PATH] = { 0 };
				GetPathDirectory(LocalDocumenPath, _MAX_PATH, LocalFilePathAbsolute);
				if (!CreateFileDirectory(LocalDocumenPath))
				{
					LOG_ERROR("产生一个冲突 : %s", LocalDocumenPath);
					continue;
				}

				if(CCopyFile(OldFilePathCollection.paths[i], LocalFilePathAbsolute))
				{
					LOG_GENERAL("将路径下的 %s 拉取到 %s 成功", OldFilePathCollection.paths[i], LocalFilePathAbsolute);
				}
				else
				{
					LOG_ERROR("将路径下的 %s 拉取到 %s 失败", OldFilePathCollection.paths[i], LocalFilePathAbsolute);
				}
				double Progress = ((double)i + 1.0) / (double)(OldFilePathCollection.index + 1) * 100;
				printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
				printf("%4.1f%%", Progress);
			}
			printf("\n");
			DestroyCArrayString(&CString);
			DestroyCArrayString(&CFile);
		}
		else if (strstr(InputBuf, "ssh-keygen -t rsa -C"))
		{
			// RSA加密
			_NOT_COMPLETE_

			LOG_ERROR("ssh-keygen -t rsa -C Not Complete");
		}
		else if (strstr(InputBuf, "git --help"))
		{
			LOG_GENERAL("user.Name 账户");
			LOG_GENERAL("user.Mail 电子邮件");
		}
		else if (strstr(InputBuf, "git commit"))
		{
			CArrayString CString;
			InitCArrayString(&CString);
			DismantlingCArrayString(InputBuf, " ", &CString);
			char const* Tips = GetCArrayString(3, &CString);
			RemoveCharEnd(Tips, '\n');

			InitGitCommit(&NewCommit);
			char TimeBuf[TIMEBUF_SIZE];
			GetTime(TimeBuf);
			strcpy_s(NewCommit.Date, TIMEBUF_SIZE, TimeBuf);
			strcpy_s(NewCommit.Name, _MAX_PATH, NewUser.Name);
			strcpy_s(NewCommit.Commit, _MAX_PATH, Tips);
			CreateGUID(&NewCommit.Guid);

			strcpy(Commits.Commits[Commits.Size].Name, NewCommit.Name);
			strcpy(Commits.Commits[Commits.Size].Commit, NewCommit.Commit);
			strcpy(Commits.Commits[Commits.Size].Date, NewCommit.Date);
			Commits.Commits[Commits.Size].Guid.a = NewCommit.Guid.a;
			Commits.Commits[Commits.Size].Guid.b = NewCommit.Guid.b;
			Commits.Commits[Commits.Size].Guid.c = NewCommit.Guid.c;
			Commits.Commits[Commits.Size++].Guid.d = NewCommit.Guid.d;


			char CommitString[_MAX_PATH] = { 0 };
			GitCommitToString(CommitString, &NewCommit);
			if (_access(GetGitProjectVersionListFileName(), 0) == 0)
			{
				AddFileBuf(GitProjectVersionListFileName, CommitString);

			}
			else
			{
				AddNewFileBuf(GitProjectVersionListFileName, CommitString);
			}

			char VersionString[8192] = { 0 };
			GitFileVersionsToString(VersionString, &ProjectVersion);
			AddNewFileBuf(GetGitProjectVersionFileName(), VersionString);

			LOG_GENERAL("Version commit with information : %s", Tips);
		}
		else if (strstr(InputBuf, "git push -u origin master"))
		{
			if (GitClientConnect(GitRemoteOrigin, GetGitExecutePath()))
			{

				GitIsServerExistType(GitRemoteOrigin);
				GitIsServerExistContent(GitRemoteOrigin);
				if (RemovedFileBuffer[0] != '\0')
				{
					if (GitSendProtocolType(GitRemoteOrigin, VERSION_DELETE, SERVER_TYPE_URL) &&
						GitSendContent(GitRemoteOrigin, RemovedFileBuffer, SERVER_CONTENT_URL))
					{
						LOG_SUCCESS("Delete file send success!");
					}
					memset(RemovedFileBuffer, 0, sizeof(RemovedFileBuffer));
				}

				for (int i = 0; i < ToSendFiles.Size; i++)
				{
					if (GitSend(ToSendFiles.Paths[i].DestPath, ToSendFiles.Paths[i].SrcPath))
					{
						LOG_GENERAL("Copy from %s to %s successed!", ToSendFiles.Paths[i].SrcPath, ToSendFiles.Paths[i].DestPath);
					}
					else
					{
						LOG_ERROR("Copy from %s to %s failed!", ToSendFiles.Paths[i].SrcPath, ToSendFiles.Paths[i].DestPath);
					}
					double Progress = ((double)i + 1.0) / (double)ToSendFiles.Size * 100;
					printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
					printf("%4.1f%%", Progress);
				}
				printf("\n");
				InitGitPath2Ds(&ToSendFiles);

				GitIsServerExistType(GitRemoteOrigin);
				GitIsServerExistContent(GitRemoteOrigin);
				//版本发送
				if (GitSendProtocolType(GitRemoteOrigin, COMMIT, SERVER_TYPE_URL))
				{
					char ContentBuffer[8192] = { 0 };
					GitCommitToString(ContentBuffer, &NewCommit);
					if (GitSendContent(GitRemoteOrigin, ContentBuffer, SERVER_CONTENT_URL))
					{
						LOG_SUCCESS("Send Commit Data Success");
					}
				}

				GitIsServerExistType(GitRemoteOrigin);
				GitIsServerExistContent(GitRemoteOrigin);
				//文件列表发送
				if (GitSendProtocolType(GitRemoteOrigin, COMMIT_FILE_VERSIONS_LIST, SERVER_TYPE_URL))
				{
					char ContentBuffer[8192] = { 0 };
					GitFileVersionsToString(ContentBuffer, &ProjectVersion);
					if (GitSendContent(GitRemoteOrigin, ContentBuffer, SERVER_CONTENT_URL))
					{
						LOG_SUCCESS("Send Version Data Success");
					}
				}
			}
			else
			{
				LOG_ERROR("Server Connect Failed!");
			}
		}
		else if (strstr(InputBuf, "git pull"))
		{
			//迭代版本
			VersionIterativeUpdate();
		}
		else
		{
			RemoveCharEnd(InputBuf, '\n');
			LOG_WARNING("Command Not Found : %s", InputBuf);
			LOG_WARNING("You can use git --help for more details");
		}
	}
}

void EngineExit(void)
{
	SaveConfig();
	GitDeleteMutex();
	printf("engine exit \r\n");
}

int main()
{
	//引擎初始化
	EngineInit();

	//主线程循环工作
	while(!bExit)
	{
		Sleep(1000);
	}

	//退出
	EngineExit();

	return 0;
}


