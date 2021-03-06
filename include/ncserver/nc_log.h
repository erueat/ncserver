/*
MIT License

Copyright (c) 2019 GIS Core R&D Department, NavInfo Co., Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#if !defined(WIN32)
#include <syslog.h>
#endif

#include <cstdarg>

namespace ncserver
{

#if defined(WIN32)
	enum LogLevel
	{
		LogLevel_debug = 7,
		LogLevel_info = 6,
		LogLevel_notice = 5,
		LogLevel_warning = 4,
		LogLevel_error = 3,
		LogLevel_crit = 2,
		LogLevel_alert = 1,
		LogLevel_emerg = 0
	};
#else
	enum LogLevel
	{
		LogLevel_debug = LOG_DEBUG,
		LogLevel_info = LOG_INFO,
		LogLevel_notice = LOG_NOTICE,
		LogLevel_warning = LOG_WARNING,
		LogLevel_error = LOG_ERR,
		LogLevel_crit = LOG_CRIT,
		LogLevel_alert = LOG_ALERT,
		LogLevel_emerg = LOG_EMERG
	};
#endif

	const char* LogLevel_toString(LogLevel o);

	class NcLogDelegate
	{
	public:
		virtual void nclogWillOutputMessage(bool hasHeader, const char* message) = 0;
	};

	class NcLog
	{
	public:
		static NcLog& instance();

		/**
			@brief
				Use Linux SIGNAL control the log level.
				The relation between the Linux SIGNAL and log level:
					SIGRTMIN + 1(35) : LOG_DEBUG
					SIGRTMIN + 2(36) : LOG_INFO
					SIGRTMIN + 3(37) : LOG_NOTICE
					SIGRTMIN + 4(38) : LOG_WARNING
					SIGRTMIN + 5(39) : LOG_ERR
					SIGRTMIN + 6(40) : LOG_CRIT
					SIGRTMIN + 7(41) : LOG_ALERT
					SIGRTMIN + 8(42) : LOG_EMERG
				Log level reference to linux vsyslog supported log level.
			@note
				If you want to use the signal to control the log level,
				this function must to be called.
			@example
				ncserver::_registerUpdateLogLevelSignal();
				NcLog::instance().init(@serverName, @logLevel);
		*/
		void registerUpdateLogLevelSignal();

		/**
			@note
				This function need to be called before used the marco ASYNC_LOG_*.
				Usually, it is called when the program is initialized.
			@example
				NcLog::instance().init(@serverName, @logLevel);
		*/
		void init(const char* serverName, LogLevel logLevel);

		/*
			@note Log size limit is 64k
		*/
		void log(LogLevel level, const char* file, int line, const char* func, const char *format, ...);

		/*
			@note Log size limit is 64k
		*/
		void rawLog(const char *format, ...);

		// default = LogLevel_error
		LogLevel logLevel() { return m_logLevel; }
		void setLogLevel(LogLevel logLevel) { m_logLevel = logLevel; }

		void setDelegate(NcLogDelegate* d) { m_delegate = d; }

	private:
		NcLog();
		~NcLog();

		void write(LogLevel logLevel, const char *format, ...);
		void _log(LogLevel logLevel, const char* header, size_t headerSize, const char* format, va_list argList);

		LogLevel m_logLevel;
		NcLogDelegate* m_delegate;
	};

} // namespace ncserver


// raw logs would be logged 'AS IS' and would always be logged
#define ASYNC_RAW_LOG(fmt, ...) do { \
	ncserver::NcLog::instance().rawLog(fmt, ##__VA_ARGS__); \
	}while (0)

// other logs would be logged with a header of format '__FILE__(__LINE__): LOG_LEVEL_STRING: [__FUNCTION__] '
#define ASYNC_LOG(logLevel, fmt, ...) do { \
	ncserver::NcLog::instance().log(logLevel, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
	} while (0)

#define ASYNC_LOG_DEBUG(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_debug, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_INFO(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_info, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_NOTICE(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_notice, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_WARNING(fmt, ...) ASYNC_LOG(ncserver::LogLevel_warning, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_ERR(fmt, ...)		ASYNC_LOG(ncserver::LogLevel_error, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_CRIT(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_crit, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_ALERT(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_alert, fmt, ##__VA_ARGS__)
#define ASYNC_LOG_EMERG(fmt, ...)	ASYNC_LOG(ncserver::LogLevel_emerg, fmt, ##__VA_ARGS__)
