#pragma once

#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <dxgi.h>
#include <d3d11.h>

#define __NARESOLVER_ONLY_API
#include <NaLibrary/NaResolver/NaResolver.h>

void Unload();

#include <NaLibrary/NaLogger/NaLogger.h>
#include "Config/Logger.h"

inline NaLogger logger = NaLogger(
	NaLoggerLevel_All, "[ NaDUGR ]"	, new Config::Logger::LoggerCallbackConfig()
);

#include "Core/Core.h"

inline HMODULE module;

bool Il2CppResolverInit();
void Run();
