#pragma once

#define CINTERFACE

#include <ddraw.h>
#include <fstream>
#include <type_traits>

#define LOG_ONCE(msg) \
	static bool isAlreadyLogged##__LINE__ = false; \
	if (!isAlreadyLogged##__LINE__) \
	{ \
		Compat::Log() << msg; \
		isAlreadyLogged##__LINE__ = true; \
	}

std::ostream& operator<<(std::ostream& os, const char* str);
std::ostream& operator<<(std::ostream& os, const unsigned char* data);
std::ostream& operator<<(std::ostream& os, const WCHAR* wstr);
std::ostream& operator<<(std::ostream& os, const RECT& rect);
std::ostream& operator<<(std::ostream& os, HDC__& dc);
std::ostream& operator<<(std::ostream& os, HWND__& hwnd);
std::ostream& operator<<(std::ostream& os, const DDSCAPS& caps);
std::ostream& operator<<(std::ostream& os, const DDSCAPS2& caps);
std::ostream& operator<<(std::ostream& os, const DDPIXELFORMAT& pf);
std::ostream& operator<<(std::ostream& os, const DDSURFACEDESC& sd);
std::ostream& operator<<(std::ostream& os, const DDSURFACEDESC2& sd);
std::ostream& operator<<(std::ostream& os, const CWPRETSTRUCT& cwrp);

template <typename T>
typename std::enable_if<std::is_class<T>::value && !std::is_same<T, std::string>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& t)
{
	return os << static_cast<const void*>(&t);
}

template <typename T>
typename std::enable_if<std::is_class<T>::value, std::ostream&>::type
operator<<(std::ostream& os, T* t)
{
	return t ? (os << *t) : (os << "null");
}

template <typename T>
std::ostream& operator<<(std::ostream& os, T** t)
{
	return t ? (os << reinterpret_cast<void*>(t) << '=' << *t) : (os << "null");
}

namespace Compat
{
	class Log
	{
	public:
		Log();
		~Log();

		template <typename T>
		Log& operator<<(const T& t)
		{
			s_logFile << t;
			return *this;
		}

	protected:
		template <typename... Params>
		Log(const char* prefix, const char* funcName, Params... params) : Log()
		{
			s_logFile << prefix << ' ' << funcName << '(';
			toList(params...);
			s_logFile << ')';
		}

	private:
		void toList()
		{
		}

		template <typename Param>
		void toList(Param param)
		{
			s_logFile << param;
		}

		template <typename Param, typename... Params>
		void toList(Param firstParam, Params... remainingParams)
		{
			s_logFile << firstParam << ", ";
			toList(remainingParams...);
		}

		static std::ofstream s_logFile;
	};

#ifdef _DEBUG
	class LogEnter : private Log
	{
	public:
		template <typename... Params>
		LogEnter(const char* funcName, Params... params) : Log("-->", funcName, params...)
		{
		}
	};

	class LogLeave : private Log
	{
	public:
		template <typename... Params>
		LogLeave(const char* funcName, Params... params) : Log("<--", funcName, params...)
		{
		}

		template <typename Result>
		void operator<<(const Result& result)
		{
			static_cast<Log&>(*this) << " = " << std::hex << result << std::dec;
		}
	};
#else
	class LogEnter
	{
	public:
		template <typename... Params> LogEnter(const char*, Params...) {}
		template <typename Result> void operator<<(const Result&) {}
	};

	typedef LogEnter LogLeave;
#endif
}
