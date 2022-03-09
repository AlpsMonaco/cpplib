#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <fstream>
#include <string>
#include <utility>
#include "time.hpp"

namespace logs
{
#ifdef _WIN32
	const char *endl = "\r\n";
#else
	const char *endl = "\n";
#endif

	template <typename T>
	class LoggerStream
	{
	public:
		LoggerStream(LoggerStream &&loggerstream);
		LoggerStream(const T *t, const std::string &buffer = "");
		LoggerStream(const LoggerStream &loggerstream) = delete;
		LoggerStream &operator=(const LoggerStream &) = delete;
		~LoggerStream();
		LoggerStream &operator<<(const std::string &content);
		LoggerStream &operator=(LoggerStream &&loggerstream);

	protected:
		std::string buffer;
		const T *pt;
	};

	class Logger
	{
	public:
		Logger(const std::string &logname);
		Logger(std::string &&logname);
		~Logger();

		void Write(const std::string &content);
		void Write(const std::string &content) const;
		void Close();
		LoggerStream<const Logger> operator<<(const std::string &rhs);

	protected:
		bool ReopenHandle(const std::string &date);
		epoch::Time time;
		std::string logname;
		std::ofstream ofs;
	};

	Logger::Logger(const std::string &logname) : time(), logname(logname), ofs() {}
	Logger::Logger(std::string &&logname) : time(), logname(std::forward<std::string>(logname)), ofs() {}
	Logger::~Logger() { this->Close(); }

	void Logger::Write(const std::string &content)
	{
		static std::string lastdate;
		time.Now();
		if (lastdate != time.StandardDate())
		{
			lastdate = time.StandardDate();
			this->ReopenHandle(lastdate);
		}
		if (!this->ofs.is_open())
		{
			if (!this->ReopenHandle(lastdate))
				return;
		}
		ofs << '[' << time.StandTime() << "] " << content << std::endl;
	}

	void Logger::Write(const std::string &content) const { const_cast<Logger &>(*this).Write(content); }
	void Logger::Close()
	{
		if (this->ofs.is_open())
			this->ofs.close();
	}

	LoggerStream<const Logger> Logger::operator<<(const std::string &rhs) { return LoggerStream<const Logger>(this, rhs); }

	bool Logger::ReopenHandle(const std::string &date)
	{
		if (this->ofs.is_open())
			this->ofs.close();
		ofs.open(logname + " " + date + ".log", std::ios_base::app | std::ios_base::binary);
		return ofs.is_open();
	}

	template <typename T>
	LoggerStream<T>::LoggerStream(LoggerStream &&loggerstream)
	{
		this->buffer(std::move(loggerstream.buffer));
		this->pt = loggerstream.pt;
		loggerstream.pt = nullptr;
	}

	template <typename T>
	LoggerStream<T>::LoggerStream(const T *pt, const std::string &buffer) : pt(pt), buffer(buffer) {}

	template <typename T>
	LoggerStream<T>::~LoggerStream()
	{
		if (buffer.size() > 0)
		{
			if (this->pt != nullptr)
			{
				this->pt->Write(buffer);
			}
		}
	}

	template <typename T>
	LoggerStream<T> &LoggerStream<T>::operator=(LoggerStream<T> &&rhs)
	{
		this->buffer(std::move(rhs.buffer));
		this->pt = rhs.pt;
		rhs.pt = nullptr;
	}

	template <typename T>
	LoggerStream<T> &LoggerStream<T>::operator<<(const std::string &content)
	{
		this->buffer += content;
		return *this;
	}
}
#endif // __LOG_HPP__