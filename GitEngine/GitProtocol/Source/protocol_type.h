#pragma once


typedef enum
{
	NONE = 0,
	HELLO,
	HI,
	COMMIT,
	COMMIT_FILE_VERSIONS_LIST,
	VERSION_LOG,
	VERSION_STATUS,
	SERVER_ERROR,
	VERSION_PROTOCOL,
	VERSION_LIST,
	VERSION_DELETE,
	VERSION_NEXT,
	VERSION_NEXT_COMPLETE,
	VERSION_CLASH,
	CLASH_CONTENT,
	NO_CLASH
} EGitProtocolType;
