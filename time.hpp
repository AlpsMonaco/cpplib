#ifndef __TIME_HPP__
#define __TIME_HPP__
#include <ctime>
#include <memory.h>

namespace epoch
{
	template <size_t buffersize>
	class TimeBuf
	{
	public:
		static constexpr int buffersize = buffersize;

		TimeBuf() : t(::time(NULL)), tm{0, 0, 0, 0, 0, 0, 0, 0, 0} { memset(this->buf, 0, buffersize); }
		TimeBuf(const TimeBuf &rhs) : t(rhs.t), tm(rhs.tm) { memcpy(this->buf, rhs.buf, buffersize); }

		// update Time instance to current time.
		void Now() { this->t = ::time(NULL); }
		const char *Format(const char *format)
		{
			localtime_s(&this->tm, &t);
			strftime(this->buf, buffersize, format, &this->tm);
			return this->buf;
		}

		inline const char *StandTime() { return this->Format("%Y-%m-%d %H:%M:%S"); }
		inline const char *StandardDate() { return this->Format("%Y-%m-%d"); }

	protected:
		time_t t;
		tm tm;
		char buf[buffersize];
	};

	using Time = TimeBuf<64>;
}

#endif // __TIME_HPP__