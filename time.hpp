#ifndef __TIME_HPP__
#define __TIME_HPP__
#include <ctime>
#include <memory.h>

namespace epoch
{
	template <size_t buffersize = 64>
	class Time
	{
	public:
		static constexpr int buffersize = buffersize;

		Time() : t(::time(NULL)), tm{0, 0, 0, 0, 0, 0, 0, 0, 0} { memset(this->buf, 0, buffersize); }

		// update Time instance to current time.
		void Now() { this->t = ::time(NULL); }
		const char *Format(const char *format)
		{
			localtime_s(&this->tm, &t);
			strftime(this->buf, buffersize, format, &this->tm);
			return this->buf;
		}

	protected:
		time_t t;
		tm tm;
		char buf[buffersize];
	};

}

#endif // __TIME_HPP__