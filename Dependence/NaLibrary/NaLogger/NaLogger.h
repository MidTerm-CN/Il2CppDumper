//**************************************//
// NaLogger - simple logger library		//
// Author: MidTerm                   	//
// Version: v1.0.0						//
// License: MIT                         //
//**************************************//

#pragma once
#include <string>
#include <vector>

typedef int NaLoggerLevel;
typedef int NaLogItemLevel;
typedef long long NaLoggerTimestamp;

enum NaLoggerLevel_
{
	NaLoggerLevel_None,
	NaLoggerLevel_Fatal,
	NaLoggerLevel_Error,
	NaLoggerLevel_Warning,
	NaLoggerLevel_Info,
	NaLoggerLevel_Debug,
	NaLoggerLevel_All
};

enum NaLogItemLevel_
{
	NaLogItemLevel_Fatal,
	NaLogItemLevel_Error,
	NaLogItemLevel_Warning,
	NaLogItemLevel_Info,
	NaLogItemLevel_Debug
};

struct NaLogItem
{
	std::string message;
	NaLoggerTimestamp timestamp;
	NaLogItemLevel level;
};

struct NaLoggerCallbacks
{
	virtual void OnLog(class NaLogger* self, NaLogItem item) = 0;
	virtual void OnClear(class NaLogger* self) = 0;
	virtual void OnCreate(class NaLogger* self) = 0;
	virtual void OnDestroy(class NaLogger* self) = 0;
};

class NaLogger
{
public:
	std::string name;
	NaLoggerLevel level;
	NaLoggerTimestamp creationTime;
	NaLoggerCallbacks* callbacks;
	
	NaLogger(NaLoggerLevel level, std::string name = "[NaLogger]", NaLoggerCallbacks* callbacks = {});
	NaLogger(NaLoggerLevel level, NaLoggerCallbacks* callbacks = {});
	~NaLogger();
	
	void Clear();

	void Log(NaLogItemLevel level, std::string message);
	void LogInfo(std::string message, ...);
	void LogWarning(std::string message, ...);
	void LogError(std::string message, ...);
	void LogFatal(std::string message, ...);
	void LogDebug(std::string message, ...);
};