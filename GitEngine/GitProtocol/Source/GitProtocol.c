#include "GitProtocol.h"

#include "../../AmeMilizeLibrary/AmeMilizeLibrary/CLibrary/CoreMinimal/FileOperate/Public/FileOperate.h"
void GitInitMutex(void)
{
	InitializeCriticalSection(&Mutex);
}

void GitLock(void)
{
	EnterCriticalSection(&Mutex);
}

void GitUnlock(void)
{
	LeaveCriticalSection(&Mutex);
}

void GitDeleteMutex(void)
{
	DeleteCriticalSection(&Mutex);
}


void InitNetworkDataBufferAndProtocol(void)
{
	ReceiveProtocol = NONE;
	memset(NetworkDataBuffer, 0, 512);
}

bool GitClientConnect(char const* Url, char const* Address)
{
	if (GitSendProtocolType(Url, HELLO,SERVER_TYPE_URL)) //发送握手报文成功
	{
		GitSendContent(Url, Address, SERVER_CONTENT_URL);

		EGitProtocolType t = NONE;

		char Buffer[512];
		GitReceive(&t, Buffer);

		if (t == HI)
		{
			return true;
		}
	}
	return false;
}

bool GitServerConnect(char const* Url)
{
	if (GitSendProtocolType(Url, HI, CLIENT_TYPE_URL)) //发送握手报文成功
	{
		EGitProtocolType t = NONE;
		GitReceive(&t, NULL);
	}
	return true;
}

bool GitSend(char const* Url, char const* LocalPath)
{
	if (CCopyFile(LocalPath, Url) != 0)
	{
		return true;
	}
	return false;
}

void GitReceive(EGitProtocolType* Type, char* Buffer)
{
	SuspendThread(hWorkThread);
	GitLock();
	//获取协议及内容
	if (NetworkDataBuffer[0] != '\0')
	{
		strcpy(Buffer, NetworkDataBuffer);
	}
	*Type = ReceiveProtocol;

	//清空缓冲
	InitNetworkDataBufferAndProtocol();
	GitUnlock();
}

bool GitSendContent(char const* Url, char const* Buffer, char const* ContentUrl)
{
	//准备传输文件
	char LocalPathBuffer[BUF_SIZE] = { 0 };
	_getcwd(LocalPathBuffer, BUF_SIZE);
	strcat_s(LocalPathBuffer, BUF_SIZE, "\\content.protocol");


	if (CCreateFile(LocalPathBuffer))
	{
		AddFileBuf(LocalPathBuffer, Buffer);

		char urlParam[BUF_SIZE] = { 0 };
		strcpy_s(urlParam, BUF_SIZE, Url);
		strcat_s(urlParam, BUF_SIZE, ContentUrl);

		if (GitSend(urlParam, LocalPathBuffer))
		{
			remove(LocalPathBuffer);
			return true;
		}
	}
	return false;
}
bool GitSendProtocolType(char const* const Url, EGitProtocolType Type, char const* const TypeUrl)
{
	char LocalPathBuffer[BUF_SIZE] = { 0 };
	_getcwd(LocalPathBuffer, BUF_SIZE);
	strcat_s(LocalPathBuffer, BUF_SIZE, "\\type.protocol");

	char Buffer[BUF_SIZE] = { 0 };
	if (CCreateFile(LocalPathBuffer))
	{
		_itoa_s((int)Type, Buffer, BUF_SIZE, 10);
		AddFileBuf(LocalPathBuffer, Buffer);

		char urlParam[BUF_SIZE] = { 0 };
		strcpy_s(urlParam, BUF_SIZE, Url);
		strcat_s(urlParam, BUF_SIZE, TypeUrl);

		if (GitSend(urlParam, LocalPathBuffer))
		{
			remove(LocalPathBuffer);
			return true;
		}
	}
	return false;
}

void GitProtocolFileIsExist(char const* Url)
{
	bool bExit = true;
	while (bExit)
	{
		if (_access(Url, 0) == 0)
		{
			Sleep(100);
		}
		else
		{
			bExit = false;
		}
	}
}

void GitIsServerExistType(char const* Url)
{
	char urlParam[BUF_SIZE] = { 0 };
	strcpy_s(urlParam, BUF_SIZE, Url);
	strcat_s(urlParam, BUF_SIZE, SERVER_TYPE_URL);

	GitProtocolFileIsExist(urlParam);
}

void GitIsServerExistContent(char const* Url)
{
	char urlParam[BUF_SIZE] = { 0 };
	strcpy_s(urlParam, BUF_SIZE, Url);
	strcat_s(urlParam, BUF_SIZE, SERVER_CONTENT_URL);

	GitProtocolFileIsExist(urlParam);
}