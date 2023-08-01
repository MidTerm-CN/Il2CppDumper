#pragma once

namespace Config
{
	namespace Logger
	{
		struct LoggerCallbackConfig : NaLoggerCallbacks
		{
			std::ofstream logFile;

			const std::unordered_map<NaLogItemLevel, const char*> logLevelNames = {
				{ NaLogItemLevel_Fatal, "[FATAL]" },
				{ NaLogItemLevel_Error, "[ERROR]" },
				{ NaLogItemLevel_Warning, "[WARNING]" },
				{ NaLogItemLevel_Info, "[INFO]" },
				{ NaLogItemLevel_Debug, "[DEBUG]" }
			};

			const std::unordered_map<NaLogItemLevel, int> logLevelColors = {
				{ NaLogItemLevel_Fatal, FOREGROUND_RED | FOREGROUND_INTENSITY },
				{ NaLogItemLevel_Error, FOREGROUND_RED | FOREGROUND_INTENSITY },
				{ NaLogItemLevel_Warning, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY },
				{ NaLogItemLevel_Info, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
				{ NaLogItemLevel_Debug, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE }
			};

			void SetConsoleTextColor(NaLogItemLevel level = -1)
			{
				int color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				if (level != -1)
				{
					color = logLevelColors.at(level);
				}
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
			}

			const char* FormatTimestamp(const NaLoggerTimestamp& timestamp, bool file = false)
			{
				static char buffer[64];
				time_t time = timestamp;
				struct tm timeinfo;
				localtime_s(&timeinfo, &time);
				strftime(buffer, sizeof(buffer), ( file ? "%Y-%m-%d %H-%M-%S" : "%Y-%m-%d %H:%M:%S"), &timeinfo);
				return buffer;
			}

			void OnLog(NaLogger* self, NaLogItem item) override
			{
				const char* name = self->name.c_str();
				const char* message = item.message.c_str();
				if (logFile.is_open()) logFile << name << " [" << FormatTimestamp(item.timestamp) << "] " << logLevelNames.at(item.level) << " " << message << std::endl;
				if (item.level >= self->level) return;
				SetConsoleTextColor(item.level);
				printf("%s [%s] %s %s\n", name, FormatTimestamp(item.timestamp), logLevelNames.at(item.level), message);
				SetConsoleTextColor();
			}

			void OnCreate(NaLogger* self) override
			{
				AllocConsole();
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				self->LogDebug("Create success");

				char path[MAX_PATH];
				GetModuleFileNameA(NULL, path, MAX_PATH);
				std::filesystem::path gamePath = path;
				gamePath = gamePath.parent_path();
				std::filesystem::path logPath = gamePath / "dump - logs";
				if (!std::filesystem::exists(logPath)) std::filesystem::create_directory(logPath);
				std::filesystem::path logFilePath = logPath / (std::string(FormatTimestamp(self->creationTime, true)) + ".log");
				logFile = std::ofstream(logFilePath, std::ios::out | std::ios::app);
				self->LogDebug("Log file created at %s", logFilePath.string().c_str());
				if (!logFile.is_open()) self->LogError("Failed to open log file");
			}

			void OnDestroy(NaLogger* self) override
			{
				self->LogDebug("Destroy...");
				FreeConsole();
				if (logFile.is_open()) logFile.close();
				Unload();
			}
		};
	}
}