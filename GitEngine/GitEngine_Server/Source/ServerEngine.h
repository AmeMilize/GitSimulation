#pragma once

#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/CoreMinimal.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Guid/Public/Guid.h"

#define TIMEBUF_SIZE 256

const char GitServerRepositoryPath[_MAX_PATH] = "C:\\Users\\11692\\Desktop\\GitSimulation\\TestArea\\RemoteRepository";
char GitServerCachePath[_MAX_PATH] = { 0 };
char GitProjectVersionListFileName[_MAX_PATH] = { 0 };
char GitCurAllFileVersionFileName[_MAX_PATH] = { 0 };
char GitPath[_MAX_PATH] = { 0 };

typedef struct
{
	int Size;
	char addr[1024][_MAX_PATH];
} GitConnectedClients;
GitConnectedClients ConnectedClients;

typedef struct GitCommit
{
	char Name[_MAX_PATH];
	char Commit[_MAX_PATH];
	char Date[TIMEBUF_SIZE];

	GUIDStruct Guid;
} GitCommit;
void InitGitCommit(GitCommit* OutCommit);
char* GitCommitToString(char* Buffer, const GitCommit* Commit);
void StringToGitCommit(GitCommit* const Commit, char* Buffer);

typedef struct GitCommits
{
	int Size;
	GitCommit Commits[1024];
} GitCommits;
GitCommits Commits;
void InitGitCommits(GitCommits* OutCommits);
void StringToGitCommits(GitCommits* const OutCommits, char const* const InBuffer);
void GitCommitsToString(char* const OutBuffer, GitCommits const* const InCommits);
int GetIndexByCommit(GitCommit const** OutCommit, char const* const ClientCRC, GitCommits const* const InCommits);

//单个文件版本
typedef struct
{
	char FileName[_MAX_PATH];
	unsigned int FileSize;
	GUIDStruct CRC;
} GitFileVersion;


//单文件多版本树
typedef struct
{
	unsigned int Size;
	GitFileVersion FileVersionList[1024];
} GitProjectVersion;
void InitProjectVersion(GitProjectVersion* const OutVersions);
void GitProjectVersionToString(char* const OutBuffer, GitProjectVersion const* const InFileVersions);
void StringToProjectVersion(GitProjectVersion* const OutFileVersions, char const* const InBuffer);
void CompareList(GitProjectVersion* OutVersion, GitProjectVersion const* const InVersionA, GitProjectVersion const* const InVersionB);

//多文件多版本树
typedef struct
{
	unsigned int Size;
	GitProjectVersion ProjectVersionList[1024];
} GitProjectVersionList;
GitProjectVersionList ProjectVersionList;
void InitProjectVersionList(GitProjectVersionList* OutProjectVersionList);
void CommitsToProjectVersionList(GitProjectVersionList* const OutVersionList, const GitCommits* const InCommits);

//路径获取
char const* GetGitServerRepositoryPath(void);
char const* GetGitProjectVersionListFileName(void);
char const* GetGitProjectVersionFileName(void);
char const* GetGitPath(void);
char const* GetGitServerCachePath(void);

//通讯
unsigned char GetProtocolType(void);
void GetProtocolContent(char* Buffer);

//EngineCore
bool GitIsExist(char const* NewClientAddress);
void ClearCache(void);
void GitAddClient(char const* const NewClient);
void InitServer(void);
void ListeningServer(void);
void CloseServer(void);
