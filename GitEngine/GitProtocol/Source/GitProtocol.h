#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "protocol_type.h"
#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/Core/CoreMinimal.h"


#define SERVER_TYPE_URL    "\\GitServerCache\\type.protocol"
#define SERVER_CONTENT_URL "\\GitServerCache\\content.protocol"
#define CLIENT_TYPE_URL    "\\GitClientCache\\type.protocol"
#define CLIENT_CONTENT_URL "\\GitClientCache\\content.protocol"

_CRT_BEGIN_C_HEADER
EGitProtocolType ReceiveProtocol;
char NetworkDataBuffer[8192];
CRITICAL_SECTION Mutex;

//Thread Control
HANDLE hWorkThread;
HANDLE hReceiveThread;

void GitInitMutex(void);
void GitLock(void);
void GitUnlock(void);
void GitDeleteMutex(void);
void InitNetworkDataBufferAndProtocol(void);
bool GitServerConnect(char const* Url);
bool GitClientConnect(char const* Url, char const* Address);
bool GitSend(char const* Url, char const* LocalPath);
void GitReceive(EGitProtocolType* Type, char* Buffer);
bool GitSendContent(char const* Url, char const* Buffer, char const* ContentUrl);
bool GitSendProtocolType(char const* const Url, EGitProtocolType Type, char const* const TypeUrl);

void GitProtocolFileIsExist(char const* Url);
void GitIsServerExistType(char const* Url);
void GitIsServerExistContent(char const* Url);
_CRT_END_C_HEADER