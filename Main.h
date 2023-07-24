#pragma once

#define ADAPTATION_GAME_VERSION "v1.341"
#define HACK_VERSION "v1.0.0"

#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <dxgi.h>
#include <d3d11.h>

#include <NaLibrary/NaResolver/NaResolver - Api.h>

#include <NaLibrary/NaLogger/NaLogger.h>
#include "Config/Logger.h"

inline NaLogger logger = NaLogger(
#ifdef _DEBUG
	NaLoggerLevel_All
#else
	NaLoggerLevel_Fatal
#endif
	, "[ NaDUGR ]"
	, new Config::Logger::LoggerCallbackConfig()
);

#include <UnityEngine SDK/UnityEngine.h>

#include <NaLibrary/NaHook/NaHook.h>

#include "Core/Core.h"
bool Il2CppResolverInit();
void Run();
