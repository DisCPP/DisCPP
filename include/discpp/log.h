#ifndef DISCPP_LOG_H
#define DISCPP_LOG_H

#include <fstream>
#include <utility>

#if(__STDC_WANT_LIB_EXT1__ != 1)
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <ctime>
#include <mutex>

#include "utils.h"

namespace discpp {
	namespace logger_flags {
		enum _LogFlagMode { // constants for logger options
			_LogFlagMask = 0xff
		};

		inline static constexpr _LogFlagMode DISABLE = static_cast<_LogFlagMode>(0x01);
		inline static constexpr _LogFlagMode INFO_SEVERITY = static_cast<_LogFlagMode>(0x02);
		inline static constexpr _LogFlagMode WARNING_SEVERITY = static_cast<_LogFlagMode>(0x04);
		inline static constexpr _LogFlagMode ERROR_SEVERITY = static_cast<_LogFlagMode>(0x08);
		inline static constexpr _LogFlagMode ALL_SEVERITY = static_cast<_LogFlagMode>(0x16);
		inline static constexpr _LogFlagMode DEBUG_SEVERITY = static_cast<_LogFlagMode>(0x32);
		inline static constexpr _LogFlagMode FILE_ONLY = static_cast<_LogFlagMode>(0x64);
	}

	class LogTextColor {
	public:
		inline static std::string RED = "\033[91m";
		inline static std::string GREEN = "\033[92m";
		inline static std::string YELLOW = "\033[93m";
		inline static std::string BLUE = "\033[94m";
		inline static std::string MAGENTA = "\033[95m";
		inline static std::string CYAN = "\033[96m";
		inline static std::string WHITE = "\033[97m";
		inline static std::string BLACK = "\033[30m";
		inline static std::string DARK_RED = "\033[31m";
		inline static std::string DARK_GREEN = "\033[32m";
		inline static std::string DARK_YELLOW = "\033[33m";
		inline static std::string DARK_BLUE = "\033[34m";
		inline static std::string DARK_MAGENTA = "\033[35m";
		inline static std::string DARK_CYAN = "\033[36m";
		inline static std::string DARK_WHITE = "\033[37m";
		inline static std::string DARK_GRAY = "\033[90m";
	};

	class LogHighlightColor {
	public:
		inline static std::string RED = "\033[101m";
		inline static std::string GREEN = "\033[102m";
		inline static std::string YELLOW = "\033[103m";
		inline static std::string BLUE = "\033[104m";
		inline static std::string MAGENTA = "\033[105m";
		inline static std::string CYAN = "\033[106m";
		inline static std::string WHITE = "\033[107m";
		inline static std::string BLACK = "\033[40m";
		inline static std::string DARK_RED = "\033[41m";
		inline static std::string DARK_GREEN = "\033[42m";
		inline static std::string DARK_YELLOW = "\033[43m";
		inline static std::string DARK_BLUE = "\033[44m";
		inline static std::string DARK_MAGENTA = "\033[45m";
		inline static std::string DARK_CYAN = "\033[46m";
		inline static std::string DARK_WHITE = "\033[47m";
		inline static std::string DARK_GRAY = "\033[100m";
	};

	class LogTextEffect {
	public:
		inline static std::string RESET = "\033[0m";
		inline static std::string BOLD_OR_BRIGHT = "\033[1m";
		inline static std::string FAINT = "\033[2m";
		inline static std::string ITALIC = "\033[3m";
		inline static std::string UNDERLINE = "\033[4m";
		inline static std::string SLOW_BLINK = "\033[5m";
		inline static std::string RAPID_LINK = "\033[6m";
		inline static std::string STRIKE = "\033[9m";
		inline static std::string FRAMED = "\033[51m";
		inline static std::string ENCIRCLED = "\033[52m";
		inline static std::string OVERLINED = "\033[53m";
	};

	class Logger {
	private:
		std::ofstream log_file;
		std::string file_path;
		int flags;

		std::mutex file_mutex;
		std::mutex console_mutex;

        enum class LogSeverity : int {
            SEV_INFO = 0,
            SEV_WARN = 1,
            SEV_ERROR = 2,
            SEV_DEBUG = 3
        };

		bool CanLog(const LogSeverity& sev) {
			if ((flags & logger_flags::DISABLE) == logger_flags::DISABLE) {
				return false; // Just return false if they disabled logger.
			} else if ((flags & logger_flags::ALL_SEVERITY) == logger_flags::ALL_SEVERITY && sev != LogSeverity::SEV_DEBUG) {
				return true; // If they want all severity then send it unless the severity is debug.
			} else if ((flags & logger_flags::DEBUG_SEVERITY) == logger_flags::DEBUG_SEVERITY) {
				return true; // When your debugging, you want to see everything.
			}

			switch (sev) {
			case LogSeverity::SEV_INFO:
				return (flags & logger_flags::INFO_SEVERITY) == logger_flags::INFO_SEVERITY;
			case LogSeverity::SEV_WARN:
				return (flags & logger_flags::WARNING_SEVERITY) == logger_flags::WARNING_SEVERITY;
			case LogSeverity::SEV_ERROR:
				return (flags & logger_flags::ERROR_SEVERITY) == logger_flags::ERROR_SEVERITY;
			default:
				return false;
			}
		}

		std::string SeverityToString(const LogSeverity& sev) {
            switch (sev) {
                case LogSeverity::SEV_INFO:
                    return "INFO";
                case LogSeverity::SEV_WARN:
                    return "WARN";
                case LogSeverity::SEV_ERROR:
                    return "ERROR";
                case LogSeverity::SEV_DEBUG:
                    return "DEBUG";
                default:
                    return "NUL";
            }
		}

		// Logs the text if it can be logged and prefix the text with the log time and the severity.
		void AutoLog(const LogSeverity& sev, const std::string& text) {
            if (!CanLog(sev)) return;

            time_t now_time_t = std::time(0);
            std::tm now{};
            char st[80];

#if defined(__STDC_LIB_EXT1__) || !defined(__linux__)
            localtime_s(&now, &now_time_t);
#else
            now = *localtime(&now_time_t);
#endif

            strftime(st, 80, "[%H:%M:%S]", &now);

            std::string time(st), tmp;
            tmp = time + " [" + SeverityToString(sev) + "] " + text + LogTextEffect::RESET;


            if ((flags & logger_flags::FILE_ONLY) == logger_flags::FILE_ONLY) {
                std::lock_guard<std::mutex> lock_guard(file_mutex);
                // Log to file
                log_file << tmp << std::endl;

                return;
            }

            // Log to console
            std::lock_guard<std::mutex> lock_guard(console_mutex);
            std::cout << tmp << std::endl;

            // Log to file
            std::lock_guard<std::mutex> lock(file_mutex);
            log_file << tmp << std::endl;
		}

	public:
		inline Logger(const std::string& file_path, const int& logger_flags = logger_flags::ALL_SEVERITY) : flags(logger_flags) {
			/**
			 * @brief Constructs a logger.
			 *
			 * ```cpp
			 *      log = Logger("logged.txt", logger_flags::WARNING_SEVERITY | logger_flags::ERROR_SEVERITY);
			 * ```
			 *
			 * @param file_path The log file path.
			 * @param logger_flags The flags for what gets logged.
			 *
			 * @return discpp::Logger
			 */

			// Check if the log file is empty, and if it is then open, else delete then reopen.
			std::ifstream file(file_path, std::ios::in | std::ios::binary);
			if (file.peek() == std::ifstream::traits_type::eof()) {
                log_file.open(file_path, std::ios::out | std::ios::binary);

                if (!log_file.is_open()) {
                    throw std::runtime_error("Failed to open logger file: " + file_path);
                } else {
                    this->file_path = file_path;
                }
			} else {
			    remove(file_path.c_str());

                log_file.open(file_path, std::ios::out | std::ios::binary);
                if (!log_file.is_open()) {
                    throw std::runtime_error("Failed to open logger file: " + file_path);
                } else {
                    this->file_path = file_path;
                }
			}
		}

		inline Logger(const int& logger_flags = logger_flags::ALL_SEVERITY) : flags(logger_flags) {
			/**
			 * @brief Constructs a logger.
			 *
			 * ```cpp
			 *      log = Logger(logger_flags::WARNING_SEVERITY | logger_flags::ERROR_SEVERITY);
			 * ```
			 *
			 * @param logger_flags The flags for what gets logged.
			 *
			 * @return discpp::Logger
			 */
		}

		inline void Close() {
			/**
			 * @brief Close the logger.
			 *
			 * ```cpp
			 *      bot->logger->Close();
			 * ```
			 *
			 * @return void
			 */

			if (log_file.is_open()) {
				Info("Logger \"" + file_path + "\" is closing");
				log_file.close();
			} else {
				Info("Logger \"" + file_path + "\" is closing");
			}
		}


		inline void Debug(const std::string& text) {
            /**
             * @brief Logs to console or file, maybe even both in the debug severity.
             *
             * ```cpp
             *      bot->logger->Debug(LogTextColor::GREEN + "Got response from gateway: " + response.text);
             *      // Output:
             *      // [19:23:08] [DEBUG] Got response from gateway: {"d":null,"op":11,"s":null,"t":null}
             * ```
             *
             * @param text The text to log.
             *
             * @return void
             */

            AutoLog(LogSeverity::SEV_DEBUG, text);
		}

        inline void Warn(const std::string& text) {
            /**
             * @brief Logs to console or file, maybe even both in the warn severity.
             *
             * ```cpp
             *      bot->logger->Warn(LogTextColor::YELLOW + "Bot has no permission to ban user");
             *      // Output:
             *      // [19:23:08] [WARN] Bot has no permission to ban user
             * ```
             *
             * @param text The text to log.
             *
             * @return void
             */

            AutoLog(LogSeverity::SEV_WARN, text);
        }

        inline void Error(const std::string& text) {
            /**
             * @brief Logs to console or file, maybe even both in the error severity.
             *
             * ```cpp
             *      bot->logger->Error(LogTextColor::RED + "Got non-json response from gateway!");
             *      // Output:
             *      // [19:23:08] [ERROR] Got non-json response from gateway!
             * ```
             *
             * @param text The text to log.
             *
             * @return void
             */

            AutoLog(LogSeverity::SEV_ERROR, text);
        }

        inline void Info(const std::string& text) {
            /**
             * @brief Logs to console or file, maybe even both in the error severity.
             *
             * ```cpp
             *      bot->logger->Info(LogTextColor::GREEN + "Reconnected to gateway!");
             *      // Output:
             *      // [19:23:08] [INFO] Reconnected to gateway!
             * ```
             *
             * @param text The text to log.
             *
             * @return void
             */

            AutoLog(LogSeverity::SEV_INFO, text);
        }
	};
}

#endif