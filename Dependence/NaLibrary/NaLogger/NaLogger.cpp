#include "NaLogger.h"
#include <chrono>
#include <cstdarg>

std::string GetFormat(const char* format, va_list args)
{
	char buffer[0x1024];
	vsprintf_s(buffer, format, args);
	return buffer;
}

NaLogger::NaLogger(NaLoggerLevel level, std::string name, NaLoggerCallbacks* callbacks)
{
	this->level = level;
	this->name = name;
	this->callbacks = callbacks;
	this->creationTime = time(nullptr);
	if (this->callbacks)
		this->callbacks->OnCreate(this);
}

NaLogger::NaLogger(NaLoggerLevel level, NaLoggerCallbacks* callbacks)
{
	this->level = level;
	this->name = "[NaLogger]";
	this->callbacks = callbacks;
	this->creationTime = time(nullptr);
	if (this->callbacks)
		this->callbacks->OnCreate(this);
}

NaLogger::~NaLogger()
{
	if (this->callbacks)
		this->callbacks->OnDestroy(this);
}

void NaLogger::Clear()
{
	this->logItems.clear();
	if (this->callbacks)
		this->callbacks->OnClear(this);
}

void NaLogger::Log(NaLogItemLevel level, std::string message)
{
	NaLogItem item;
	item.level = level;
	item.message = message;
	item.timestamp = time(nullptr);
	this->logItems.push_back(item);
	if (this->callbacks)
		this->callbacks->OnLog(this, item);
}

void NaLogger::LogInfo(std::string message, ...)
{
	va_list args;
	va_start(args, message);
	this->Log(NaLogItemLevel_Info, GetFormat(message.c_str(), args));
	va_end(args);
}

void NaLogger::LogWarning(std::string message, ...)
{
	va_list args;
	va_start(args, message);
	this->Log(NaLogItemLevel_Warning, GetFormat(message.c_str(), args));
	va_end(args);
}

void NaLogger::LogError(std::string message, ...)
{
	va_list args;
	va_start(args, message);
	this->Log(NaLogItemLevel_Error, GetFormat(message.c_str(), args));
	va_end(args);
}

void NaLogger::LogFatal(std::string message, ...)
{
	va_list args;
	va_start(args, message);
	this->Log(NaLogItemLevel_Fatal, GetFormat(message.c_str(), args));
	va_end(args);
}

void NaLogger::LogDebug(std::string message, ...)
{
	va_list args;
	va_start(args, message);
	this->Log(NaLogItemLevel_Debug, GetFormat(message.c_str(), args));
	va_end(args);
}

