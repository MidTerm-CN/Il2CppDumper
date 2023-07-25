#include "Main.h"

#pragma comment(lib, "d3d11.lib")

#define LOGGER_MESSAGE_BUFFER(level) \
	char buffer[1024]; \
	va_list args; \
	va_start(args, m); \
	vsprintf_s(buffer, m.c_str(), args); \
	va_end(args); \
	logger.Log(NaLogItemLevel_##level, buffer);

bool Il2CppResolverInit()
{
	NaResolver::Config config;
	config.enableLogger = true;
	config.logger.debug = [](std::string m, ...)->void { LOGGER_MESSAGE_BUFFER(Debug); };
	config.logger.info = [](std::string m, ...)->void { LOGGER_MESSAGE_BUFFER(Info); };
	config.logger.error = [](std::string m, ...)->void { LOGGER_MESSAGE_BUFFER(Error); };
	config.logger.fatal = [](std::string m, ...)->void { LOGGER_MESSAGE_BUFFER(Fatal); };
	return Il2CppResolver->Setup(config);
}

void Run()
{
	try
	{
		logger.LogInfo("Perfect loaded!");
		if (!Il2CppResolverInit())
			return;
		logger.LogDebug("Il2CppResolverInit success!");
		RunResolver();
	}
	catch (const std::exception& e)
	{
		logger.LogFatal("Exception: %s", e.what());
	}
}