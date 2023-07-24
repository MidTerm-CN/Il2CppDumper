#pragma once

namespace Config
{
	namespace Logger
	{
		struct LoggerCallbackConfig : NaLoggerCallbacks
		{
			std::ofstream logFile;

#ifdef _DEBUG
			const std::unordered_map<NaLogItemLevel, const char*> logLevelNames = {
				{ NaLogItemLevel_Fatal, "[FATAL]" },
				{ NaLogItemLevel_Error, "[ERROR]" },
				{ NaLogItemLevel_Warning, "[WARNING]" },
				{ NaLogItemLevel_Info, "[INFO]" },
				{ NaLogItemLevel_Debug, "[DEBUG]" }
			};
#endif

			const char* FormatTimestamp(const NaLoggerTimestamp& timestamp)
			{
				static char buffer[64];
				time_t time = timestamp;
				struct tm timeinfo;
				localtime_s(&timeinfo, &time);
				strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
				return buffer;
			}

#ifndef _DEBUG
			long FormatLevelToMessageIcon(const NaLogItemLevel& level)
			{
				switch (level)
				{
				case NaLogItemLevel_Info:
					return MB_ICONINFORMATION;
				case NaLogItemLevel_Warning:
					return MB_ICONWARNING;
				case NaLogItemLevel_Error:
					return MB_ICONERROR;
				case NaLogItemLevel_Fatal:
					return MB_ICONERROR;
				case NaLogItemLevel_Debug:
					return MB_ICONINFORMATION;
				default:
					return 0L;
				}
			}
#endif
			void OnLog(NaLogger* self, NaLogItem item) override
			{
				const char* name = self->name.c_str();
				const char* message = item.message.c_str();
				if (logFile.is_open()) logFile << name << " [" << FormatTimestamp(item.timestamp) << "] " << logLevelNames.at(item.level) << " " << message << std::endl;
				if (item.level >= self->level) return;
#ifdef _DEBUG
				printf("%s [%s] %s %s\n", name, FormatTimestamp(item.timestamp), logLevelNames.at(item.level), message);
#else
				MessageBoxA(NULL, message, name, MB_OK | FormatLevelToMessageIcon(item.level));
#endif
			}

			void OnClear(NaLogger* self) override
			{
				self->LogDebug("Clear...");
			}

			void OnCreate(NaLogger* self) override
			{
#ifdef _DEBUG
				AllocConsole();
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif
				self->LogDebug("Create success");

				char path[MAX_PATH];
				GetModuleFileNameA(NULL, path, MAX_PATH);
				std::filesystem::path gamePath = path;
				gamePath = gamePath.parent_path();
				std::filesystem::path logPath = gamePath / "logs";
				if (!std::filesystem::exists(logPath)) std::filesystem::create_directory(logPath);
				std::filesystem::path logFilePath = logPath / (std::string(FormatTimestamp(self->creationTime)) + ".log");
				logFile = std::ofstream(logFilePath, std::ios::out | std::ios::app);
				self->LogDebug("Log file created at %s", logFilePath.string().c_str());
				if (!logFile.is_open()) self->LogError("Failed to open log file");
			}

			void OnDestroy(NaLogger* self) override
			{
				self->LogDebug("Destroy...");
#ifdef _DEBUG
				FreeConsole();
#endif
			}
		};
	}
}