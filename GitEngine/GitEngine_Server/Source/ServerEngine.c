#define _CRT_SECURE_NO_WARNINGS
#include "ServerEngine.h"

#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/FileOperate/Public/FileOperate.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/Algorithm/Public/StringAlgorithm.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/Array/Public/CArrayString.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Guid/Public/Guid.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Windows/Command/Public//WindowsSettings.h"

#include "../../GitProtocol/Source/GitProtocol.h"
void InitGitCommit(GitCommit* OutCommit)
{
	memset(OutCommit->Name, 0, _MAX_PATH);
	memset(OutCommit->Commit, 0, _MAX_PATH);
	memset(OutCommit->Date, 0, TIMEBUF_SIZE);
	InitGUID(&OutCommit->Guid);
}

char* GitCommitToString(char* Buffer, const GitCommit* Commit)
{
	char GuidBuffer[_MAX_PATH] = { 0 };
	GUIDToString(GuidBuffer, _MAX_PATH, &Commit->Guid);
	GetFormatStringS(Buffer, 512, "%s\n%s\n%s\n%s\n", Commit->Name, Commit->Commit, Commit->Date, GuidBuffer);

	return Buffer;
}

void StringToGitCommit(GitCommit* const Commit, char* Buffer)
{
	InitGitCommit(Commit);

	CArrayString CString;
	InitCArrayString(&CString);
	DismantlingCArrayString(Buffer, "\n", &CString);
	{
		char* value = GetCArrayString(0, &CString);
		strcpy(Commit->Name, value);
	}
	{
		char* value = GetCArrayString(1, &CString);
		strcpy(Commit->Commit, value);
	}
	{
		char* value = GetCArrayString(2, &CString);
		strcpy(Commit->Date, value);
	}
	{
		char* value = GetCArrayString(3, &CString);
		StringToGUID(&Commit->Guid, value);
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

void GitCommitsToString(char* const OutBuffer, GitCommits const* const InCommits)
{
	for (int i = 0; i < InCommits->Size; i++)
	{
		char TmpBuffer[1024] = { 0 };
		GitCommitToString(TmpBuffer, &InCommits->Commits[i]);

		strcat(OutBuffer, TmpBuffer);
	}
}

void InitProjectVersion(GitProjectVersion* const OutVersions)
{
	OutVersions->Size = 0;
	memset(OutVersions->FileVersionList, 0, sizeof(OutVersions->FileVersionList));
}

void GitProjectVersionToString(char* const OutBuffer, GitProjectVersion const* const InFileVersions)
{
	for (unsigned i = 0; i < InFileVersions->Size; i++)
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

void StringToProjectVersion(GitProjectVersion* const OutFileVersions, char const* const InBuffer)
{
	InitProjectVersion(OutFileVersions);

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

void InitGitCommits(GitCommits* OutCommits)
{
	OutCommits->Size = 0;
	memset(OutCommits->Commits, 0, sizeof(OutCommits->Commits));
}

void InitProjectVersionList(GitProjectVersionList* OutProjectVersionList)
{
	OutProjectVersionList->Size = 0;
	memset(OutProjectVersionList->ProjectVersionList, 0, sizeof(OutProjectVersionList->ProjectVersionList));
}

void CommitsToProjectVersionList(GitProjectVersionList* const OutVersionList, const GitCommits* const InCommits)
{
	for(int i = 0; i < InCommits->Size; i++)
	{
		char GuidBuffer[_MAX_PATH] = { 0 };
		GUIDToString(GuidBuffer, _MAX_PATH, &InCommits->Commits[i].Guid);

		char PathBuffer[_MAX_PATH] = { 0 };
		const char* const p = GetGitPath();
		GetFormatString(PathBuffer, "%s\\%s.lst", p, GuidBuffer);

		char DataBuffer[8192] = { 0 };
		GetFileBuf(PathBuffer, DataBuffer, 8192);
		StringToProjectVersion(&OutVersionList->ProjectVersionList[i], DataBuffer);
		OutVersionList->Size++;
	}
}

int GetIndexByCommit(GitCommit const** OutCommit, char const* const ClientCRC, GitCommits const* const InCommits)
{
	int index = -1;
	for(int i = 0; i < InCommits->Size; i++)
	{
		if(GuidEqualString(ClientCRC, &InCommits->Commits[i].Guid))
		{
			*OutCommit = &InCommits->Commits[i + 1];
			index = i + 1;
			break;
		}
	}

	return index;
}

void CompareList(GitProjectVersion* OutVersion, GitProjectVersion const* const InVersionA, GitProjectVersion const* const InVersionB)
{
	for (int i = 0; i < InVersionA->Size; i++)
	{
		for (int j = 0; j < InVersionB->Size; j++)
		{
			if (strcmp(InVersionA->FileVersionList[i].FileName, InVersionB->FileVersionList[j].FileName) == 0)
			{
				if (InVersionA->FileVersionList[i].FileSize != InVersionB->FileVersionList[j].FileSize)
				{
					strcpy(OutVersion->FileVersionList[OutVersion->Size].FileName, InVersionA->FileVersionList[i].FileName);
					OutVersion->FileVersionList[OutVersion->Size].FileSize = InVersionA->FileVersionList[i].FileSize;
					OutVersion->FileVersionList[OutVersion->Size].CRC.a = InVersionA->FileVersionList[i].CRC.a;
					OutVersion->FileVersionList[OutVersion->Size].CRC.b = InVersionA->FileVersionList[i].CRC.b;
					OutVersion->FileVersionList[OutVersion->Size].CRC.c = InVersionA->FileVersionList[i].CRC.c;
					OutVersion->FileVersionList[OutVersion->Size++].CRC.d = InVersionA->FileVersionList[i].CRC.d;
				}
			}
		}
	}
}

bool GitIsExist(char const* NewClientAddress)
{
	for(int i = 0; i < ConnectedClients.Size; i++)
	{
		if(strcmp(ConnectedClients.addr[i], NewClientAddress) == 0)
		{
			return true;
		}
	}

	return false;
}

const char* GetGitServerRepositoryPath()
{
	return GitServerRepositoryPath;
}

const char* GetGitProjectVersionListFileName(void)
{
	if(GitProjectVersionListFileName[0] == '\0')
	{
		strcpy(GitProjectVersionListFileName, GitPath);
		strcat(GitProjectVersionListFileName, "\\version.list");
	}
	return GitProjectVersionListFileName;
}

const char* GetGitProjectVersionFileName(void)
{
	memset(GitCurAllFileVersionFileName, 0, sizeof(GitCurAllFileVersionFileName));
	strcpy(GitCurAllFileVersionFileName, GitPath);
	strcat(GitCurAllFileVersionFileName, "\\");

	GitCommit const* LastCommit = &Commits.Commits[Commits.Size - 1];

	if(LastCommit)
	{
		char GuidBuffer[_MAX_PATH] = { 0 };
		GUIDToString(GuidBuffer, _MAX_PATH, &LastCommit->Guid);
		strcat(GitCurAllFileVersionFileName, GuidBuffer);
		strcat(GitCurAllFileVersionFileName, ".lst");
	}
	
	return GitCurAllFileVersionFileName;
}

const char* GetGitPath(void)
{
	if (GitPath[0] == '\0')
	{
		strcpy(GitPath, GitServerRepositoryPath);
		strcat(GitPath, "\\.gitX");
	}
	return GitPath;
}

const char* GetGitServerCachePath()
{
	if(GitServerCachePath[0] == '\0')
	{
		_getcwd(GitServerCachePath, _MAX_PATH);
		strcat(GitServerCachePath, "\\GitServerCache");
	}
	return GitServerCachePath;
}

void ClearCache(void)
{
	char ContentPath[_MAX_PATH] = { 0 };
	char ProtocolTypePath[_MAX_PATH] = { 0 };

	strcpy(ContentPath, GitServerCachePath);
	strcpy(ProtocolTypePath, GitServerCachePath);
	strcat(ProtocolTypePath, "\\type.protocol");
	strcat(ContentPath, "\\content.protocol");
	remove(ContentPath);
	remove(ProtocolTypePath);
}

void GitAddClient(char const* const NewClient)
{
	strcpy(ConnectedClients.addr[ConnectedClients.Size++], NewClient);
}

void InitServer(void)
{
	//全局变量初始化
	InitGitCommits(&Commits);
	InitProjectVersionList(&ProjectVersionList);
	ConnectedClients.Size = 0;
	memset(ConnectedClients.addr, 0, sizeof(ConnectedClients.addr));

	//建立目录结构
	_mkdir(GetGitServerCachePath());
	_mkdir(GetGitPath());

	//清空缓存
	ClearCache();

	//读取版本信息
	if (_access(GetGitProjectVersionListFileName(), 0) != 0)
	{
		CCreateFile(GitProjectVersionListFileName);
	}
	else
	{
		char CommitsBuffer[8192] = { 0 };
		GetFileBuf(GitProjectVersionListFileName, CommitsBuffer, 8192);
		StringToGitCommits(&Commits, CommitsBuffer);
		CommitsToProjectVersionList(&ProjectVersionList, &Commits);
	}
}

void ListeningServer(void)
{
	int count = 0;
	bool bQuit = false;
	while (!bQuit)
	{
		unsigned char const Protocol = GetProtocolType();
		printf("%d \r\n", count++);
		switch (Protocol)
		{
		case NONE:
		{
			break;
		}
		case HELLO: //客户端链接到服务端
		{

			Sleep(1000);
			char NewClientAddress[8196] = { 0 };
			GetProtocolContent(NewClientAddress);

			if (!GitIsExist(NewClientAddress))
			{
				GitAddClient(NewClientAddress);
			}

			char const* ClientAddr = ConnectedClients.addr[ConnectedClients.Size - 1];
			printf("Connect Client : %s Success! \r\n", ClientAddr);
			if (GitSendProtocolType(ClientAddr, HI, CLIENT_TYPE_URL))
			{
				printf("Send protocol to Client");
			}
			else
			{
				printf("Invalid Address");
			}

			break;
		}
		case COMMIT:
		{
			Sleep(2000);
			char ContentBuffer[8196] = { 0 };
			GetProtocolContent(ContentBuffer);
			StringToGitCommit(&Commits.Commits[Commits.Size++], ContentBuffer);
			AddFileBuf(GitProjectVersionListFileName, ContentBuffer);
			break;
		}
		case COMMIT_FILE_VERSIONS_LIST:
		{
			printf("Receive project version success!");
			Sleep(2000);
			char ContentBuffer[8196] = { 0 };
			GetProtocolContent(ContentBuffer);

			StringToProjectVersion(&ProjectVersionList.ProjectVersionList[ProjectVersionList.Size++], ContentBuffer);
			AddFileBuf(GetGitProjectVersionFileName(), ContentBuffer);
			break;
		}
		case VERSION_LOG:
		{
			Sleep(500); //等待Content
			char ClientAddr[_MAX_PATH] = { 0 };
			GetProtocolContent(ClientAddr);
			if (GitSendProtocolType(ClientAddr, VERSION_LOG, CLIENT_TYPE_URL))
			{
				char CommitsBuffer[8196] = { 0 };
				GitCommitsToString(CommitsBuffer, &Commits);

				if (GitSendContent(ClientAddr, CommitsBuffer, CLIENT_CONTENT_URL))
				{
					printf("Send to client success!");
				}
				else
				{
					printf("Send to client failed!");
				}
			}
			break;
		}
		case VERSION_STATUS:
		{
			Sleep(2000); //等待Content
			char ContentBuffer[8192] = { 0 };
			GetProtocolContent(ContentBuffer);

			if (ContentBuffer[0] != '\0')
			{
				CArrayString CString;
				InitCArrayString(&CString);
				DismantlingCArrayString(ContentBuffer, "\n", &CString);
				const char* ClientAddress = GetCArrayString(0, &CString);
				const char* ClientCRC = GetCArrayString(1, &CString);

				for (int i = 0; i < Commits.Size; i++)
				{
					if (GuidEqualString(ClientCRC, &Commits.Commits[i].Guid))
					{
						char FileVersionsBuffer[8196] = { 0 };
						GitProjectVersionToString(FileVersionsBuffer, &ProjectVersionList.ProjectVersionList[i]);

						if (GitSendProtocolType(ClientAddress, VERSION_STATUS, CLIENT_TYPE_URL)
							&& GitSendContent(ClientAddress, FileVersionsBuffer, CLIENT_CONTENT_URL))
						{
							printf("\n VERSION_STATUS: Send to Client Success !");
						}
						else
						{
							printf("\n VERSION_STATUS: Send to Client  Failed !");
							if (GitSendProtocolType(ClientAddress, SERVER_ERROR, CLIENT_TYPE_URL))
							{
								SetWindowFontRepresentColor(RED, 0);
								printf("\n SERVER_ERROR: Send to Client  Failed !");
								SetWindowFontRepresentColor(WHITE, 0);
							}
						}
					}

				}
				DestroyCArrayString(&CString);
			}
			break;
		}
		case VERSION_PROTOCOL:
		{
			Sleep(2000);
			GitCommit* LastCommit = &Commits.Commits[Commits.Size - 1];

			if (LastCommit)
			{
				char ClientAddress[8192] = { 0 };
				GetProtocolContent(ClientAddress);

				char TmpBuffer[1024] = { 0 };
				GitCommitToString(TmpBuffer, LastCommit);
				if (GitSendProtocolType(ClientAddress, VERSION_PROTOCOL, CLIENT_TYPE_URL) &&
					GitSendContent(ClientAddress, TmpBuffer, CLIENT_CONTENT_URL))
				{
					printf("\n Send newest version to client success!\r\n");
				}
			}
			break;
		}
		case VERSION_NEXT:
		{
			Sleep(2000);
			char ReceiveContent[8192] = { 0 };
			GetProtocolContent(ReceiveContent);

			CArrayString CString;
			InitCArrayString(&CString);
			DismantlingCArrayString(ReceiveContent, "\n", &CString);

			char* ClientCRC = CString.data[0].data;
			char* ClientAddress = CString.data[1].data;
			char* ClientRepositoryPath = CString.data[2].data;

			GitCommit const* NextCommit = NULL;
			int const VersionPtr = GetIndexByCommit(&NextCommit, ClientCRC, &Commits);

			if (NextCommit)
			{
				char ContentBuffer[8192] = { 0 };
				GitCommitToString(ContentBuffer, NextCommit);
				if (GitSendProtocolType(ClientAddress, VERSION_NEXT, CLIENT_TYPE_URL) &&
					GitSendContent(ClientAddress, ContentBuffer, CLIENT_CONTENT_URL))
				{
					printf("\n Send new commit to client success!\r\n");

					Sleep(1000);
					GitProjectVersion* NextVersion = &ProjectVersionList.ProjectVersionList[VersionPtr];

					for (unsigned int i = 0; i < ProjectVersionList.ProjectVersionList[VersionPtr].Size; i++)
					{
						const char* RelativePath = ProjectVersionList.ProjectVersionList[VersionPtr].FileVersionList[i].FileName;
						char ClientAbsolutePath[_MAX_PATH] = { 0 };
						char ServerAbsolutePath[_MAX_PATH] = { 0 };

						strcpy(ClientAbsolutePath, ClientRepositoryPath);
						strcat(ClientAbsolutePath, RelativePath);
						strcpy(ServerAbsolutePath, GetGitServerRepositoryPath());
						strcat(ServerAbsolutePath, RelativePath);

						CreateFileDirectory(ClientAbsolutePath);

						if (GitSend(ClientAbsolutePath, ServerAbsolutePath))
						{
							printf("\nCopy from %s to %s success!\r\n", ServerAbsolutePath, ClientAbsolutePath);
						}
						else
						{
							printf("\nCopy from %s to %s failed!\r\n", ServerAbsolutePath, ClientAbsolutePath);
						}
					}
					memset(ContentBuffer, 0, sizeof(ContentBuffer));
					GitProjectVersionToString(ContentBuffer, NextVersion);

					if (GitSendProtocolType(ClientAddress, VERSION_NEXT_COMPLETE, CLIENT_TYPE_URL) &&
						GitSendContent(ClientAddress, ContentBuffer, CLIENT_CONTENT_URL))
					{
						printf("\n Send new version to client success!\r\n");
					}
				}
			}
			break;
		}
		case VERSION_CLASH:
		{
			char ReceiveBuffer[8192] = { 0 };
			GetProtocolContent(ReceiveBuffer);

			CArrayString CString;
			InitCArrayString(&CString);

			DismantlingCArrayString(ReceiveBuffer, "|", &CString);
			char const* const ClientCRC = GetCArrayString(0, &CString);
			char const* const ClientAddress = GetCArrayString(1, &CString);
			char const* const GitVersionBuffer = GetCArrayString(2, &CString);

			GitProjectVersion ClientVersion;
			InitProjectVersion(&ClientVersion);
			StringToProjectVersion(&ClientVersion, GitVersionBuffer);

			GitCommit const* TargetCommit = NULL;
			int const CommitPtr = GetIndexByCommit(&TargetCommit, ClientCRC, &Commits);

			GitProjectVersion const* const ServerVersion = &ProjectVersionList.ProjectVersionList[CommitPtr];
			GitProjectVersion OutVersion;
			InitProjectVersion(&OutVersion);
			CompareList(&OutVersion, ServerVersion, &ClientVersion);

				if (OutVersion.Size == 0 || CommitPtr == Commits.Size - 1)
				{
					if (GitSendProtocolType(ClientAddress, NO_CLASH, CLIENT_TYPE_URL))
					{
						printf("\n No clash \n");
					}
				}
				else
				{
					char ContentBuffer[8192 * 10] = { 0 };
					GitProjectVersionToString(ContentBuffer, &OutVersion);
					if (GitSendProtocolType(ClientAddress, CLASH_CONTENT, CLIENT_TYPE_URL) &&
						GitSendContent(ClientAddress, ContentBuffer, CLIENT_CONTENT_URL))
					{
						printf("\n Clash \n");
					}
				}
			DestroyCArrayString(&CString);

			break;
		}
		case VERSION_LIST:
		{
			char ClientAddress[8192] = { 0 };
			GetProtocolContent(ClientAddress);

			CArrayString CString;
			InitCArrayString(&CString);

			char ContentBuffer[8192 * 1024] = { 0 };
			if (ProjectVersionList.Size != 0)
			{
				GitProjectVersion const* ServerVersion = NULL;
				ServerVersion = &ProjectVersionList.ProjectVersionList[ProjectVersionList.Size - 1];
				GitProjectVersionToString(ContentBuffer, ServerVersion);
			}

			if (GitSendProtocolType(ClientAddress, VERSION_LIST, CLIENT_TYPE_URL) &&
				GitSendContent(ClientAddress, ContentBuffer, CLIENT_CONTENT_URL))
			{
				printf("\n Send newest version to client success! \n");
			}


			break;
		}
		case VERSION_DELETE:
		{
			char DeleteBuffer[8192] = { 0 };
			GetProtocolContent(DeleteBuffer);

			CArrayString CString;
			InitCArrayString(&CString);

			DismantlingCArrayString(DeleteBuffer, "\n", &CString);

			for (int i = 0; i < CString.size; i++)
			{
				char TargetFileName[_MAX_PATH] = { 0 };
				strcpy(TargetFileName, GetGitServerRepositoryPath());
				strcat(TargetFileName, CString.data[i].data);
				if (remove(TargetFileName) == 0)
				{
					printf("%s delete success! \r\n", TargetFileName);
				}
			}
			break;
		}
		}
		Sleep(500);
	}
}

void CloseServer(void)
{
	
}

unsigned char GetProtocolType(void)
{
	unsigned char ProtocolNumber = 0;


	char const* p = GetGitServerCachePath();

	char PathBuffer[_MAX_PATH] = { 0 };
	strcpy_s(PathBuffer, _MAX_PATH, p);
	strcat_s(PathBuffer, _MAX_PATH, "\\type.protocol");

	if(_access(PathBuffer, 0) == 0)
	{
		char ContentBuffer[_MAX_PATH] = { 0 };

		if(GetFileBuf(PathBuffer, ContentBuffer, _MAX_PATH))
		{
			remove(PathBuffer);
		}

		ProtocolNumber = atoi(ContentBuffer);
	}

	return ProtocolNumber;
}

void GetProtocolContent(char* Buffer)
{
	char ContentFileName[_MAX_PATH];
	strcpy(ContentFileName, GitServerCachePath);
	strcat(ContentFileName, "\\content.protocol");
	if (_access(ContentFileName, 0) == 0)
	{
		if (GetFileBuf(ContentFileName, Buffer, 8192))
		{
			remove(ContentFileName);
		}
	}
}

void main()
{
	InitServer();
	ListeningServer();
	CloseServer();
}