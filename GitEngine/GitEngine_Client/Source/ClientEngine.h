#pragma once
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/CoreMinimal.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Guid/Public/Guid.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/FileOperate/Public/FileOperate.h"

#define TIMEBUF_SIZE 256
#define _NOT_COMPLETE_ printf("Not Finish Yet!");


bool bExit = false;
char GitLocalExecutePath          [_MAX_PATH] = { 0 };  //程序执行路径
char GitLocalRepositoryPath       [_MAX_PATH] = { 0 };  //本地仓库路径
char GitLocalGitPath              [_MAX_PATH] = { 0 };  //本地仓库.git路径
char GitLocalClientCachePath      [_MAX_PATH] = { 0 };  //本地接收缓存
char GitLocalUserPath             [_MAX_PATH] = { 0 };  //用户目录
char GitLocalUserFileName         [_MAX_PATH] = { 0 };  //用户文件
char GitLocalConfigPath           [_MAX_PATH] = { 0 };  //配置路径
char GitLocalConfigFileName       [_MAX_PATH] = { 0 };  //配置文件
char GitRemoteOrigin              [_MAX_PATH] = { 0 };  //远端路径
char GitRemoteRepositoryPath      [_MAX_PATH] = { 0 };  //远端仓库路径
char GitCurAllFileVersionFileName [_MAX_PATH] = { 0 };  //当前版本信息文件
char GitProjectVersionListFileName[_MAX_PATH] = { 0 };  //版本列表文件
char GitCurWorkPath               [_MAX_PATH] = { 0 };  //当前工作目录
char RemovedFileBuffer[8192 * 1024] = { 0 };

typedef enum EError
{
	GIT_SUCCESS = 0,
	GIT_LOG,
	GIT_WARNING,
	GIT_ERROR

} GitError;

typedef struct GitUser
{
	char Name[_MAX_PATH];
	char Password[_MAX_PATH];
	char Mail[_MAX_PATH];

} GitUser;
GitUser NewUser;

void ReadUser(void);
void SaveUser(void);

typedef struct GitCommit
{
	char Name[_MAX_PATH];
	char Commit[_MAX_PATH];
	char Date[TIMEBUF_SIZE];

	GUIDStruct Guid;
} GitCommit;
GitCommit NewCommit;

void InitGitCommit(GitCommit* OutCommit);
void GetGitCommit(GitCommit* OutCommit);
void StringToGitCommit(GitCommit* const OutCommit, char const* const InBuffer);
char* GitCommitToString(char* const OutBuffer, GitCommit const* const InCommit);

typedef struct GitCommits
{
	int Size;
	GitCommit Commits[1024];
} GitCommits;
GitCommits Commits;

void InitGitCommits(GitCommits* OutCommit);
void StringToGitCommits(GitCommits* const OutCommits, char const* const InBuffer);

typedef struct
{
	char SrcPath[_MAX_PATH];
	char DestPath[_MAX_PATH];
} GitPath2D;

typedef struct
{
	unsigned int Size;
	GitPath2D Paths[_MAX_PATH * 2];
} GitPath2Ds;
GitPath2Ds ToSendFiles;

void InitGitPath2Ds(GitPath2Ds* Path2Ds);

//文件版本信息
typedef struct
{
	unsigned int FileSize;
	char FileName[_MAX_PATH];

	GUIDStruct CRC;
} GitFileVersion;

//文件版本合集
typedef struct
{
	unsigned int Size;
	GitFileVersion FileVersionList[1024];
} GitProjectVersion;
GitProjectVersion ProjectVersion;

void InitProjectVersion(GitProjectVersion* FileVersions);
void AddProjectVersion(GitProjectVersion* TargetProjectVersion, char const* InPath);
void GitFileVersionsToString(char* const OutBuffer, GitProjectVersion const* const InFileVersions);
void StringToGitProjectVersion(GitProjectVersion* const OutFileVersions, char const* const InBuffer);

//版本合集列表
typedef struct
{
	unsigned int Size;
	GitProjectVersion ProjectVersionList[1024];
} GitProjectVersionList;
GitProjectVersionList ProjectVersionList;

void GetProjectVersionListByCommits(GitProjectVersionList* const OutVersionList, GitCommits const* const InCommits);

//多线程回调函数
DWORD WINAPI GitClientSend(LPVOID lpParam);
DWORD WINAPI GitClientReceive(LPVOID lpParam);

//通信
unsigned char GetProtocolType(void);
void GetProtocolContent(char* Buffer);

//路径获取
char const* GetGitExecutePath(void);
char const* GetGitLocalRepositoryPath(void);
char const* GetGitLocalClientCachePath(void);
char const* GetGitLocalConfigFileName(void);
char const* GetGitLocalUserFileName(void);
char const* GetGitRemoteRepositoryPath(void);
char const* GetGitLocalGitPath(void);
char const* GetGitProjectVersionListFileName(void);
char const* GetGitProjectVersionFileName(void);
char const* GetGitLocalUserPath(void);
char const* GetGitCurWorkPath(void);


//EngineCore
void ClearCache(void);
void VersionIterativeUpdate(void);
void CompareList(GitProjectVersion const* const ServerVersion, PathCollection const* const LocalPaths, PathCollection* const OutPaths);
void GitAddLocalData(GitProjectVersion const* const ServerVersion, char const* TargetFile);
void ShowCommandLine(void);
void CreateLocalFile(void);
void SaveConfig(void);
void ReadConfig(void);
void EngineInit(void);
void EngineLoop(void);
void EngineExit(void);