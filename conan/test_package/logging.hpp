#pragma once

#include <sstream>
#include <iostream>
#include <memory>
#include <cxxabi.h>
#include <cstring>

#ifndef _
#define _(X) (X)
#endif

class SimpleLogStream
{
public:
	static SimpleLogStream* GetInstance()
	{
		if (!instance_)
			instance_.reset(new SimpleLogStream());
		return instance_.get();
	}
	std::stringstream& _stream(std::ostream& ostr)
	{
		ostr_ = &ostr;
		ss_.str("");
		memset(prefix_buf_, 0, sizeof(prefix_buf_));
		time_t t = time(NULL);
		strftime(prefix_buf_, sizeof(prefix_buf_) - 1, "[%Y%m%dT%H%M%S] ", localtime(&t));
		prefix_len_ = strlen(prefix_buf_);
		ss_ << prefix_buf_;
		return ss_;
	}
	void operator<(std::ostream& ss)
	{
		if (align)
		{
			std::istream strm(ss.rdbuf());
			bool         first = true;
			for (std::string line; std::getline(strm, line);)
			{
				if (first)
				{
					first = false;
				}
				else
				{
					for (size_t i = 0; i < prefix_len_; ++i)
						*ostr_ << ' ';
				}
				*ostr_ << line << std::endl;
			}
		}
		else
		{
			*ostr_ << ss.rdbuf() << std::endl;
		}
	}
	static bool verbose;
	static bool align;

private:
	static std::unique_ptr<SimpleLogStream> instance_;
	SimpleLogStream()
		: ostr_(&std::cout){};
	SimpleLogStream(const SimpleLogStream&)             = delete;
	SimpleLogStream(const SimpleLogStream&&)            = delete;
	SimpleLogStream&  operator=(const SimpleLogStream&) = delete;
	std::stringstream ss_;
	std::ostream*     ostr_;
	char              prefix_buf_[20];
	uint8_t           prefix_len_;
};

class SilentNoop
{
public:
	SilentNoop() {}
	void operator&(SimpleLogStream&) {}
};

extern std::ostream& ilog;
extern std::ostream& elog;
extern std::ostream& vlog;

// clang-format off
#define VLOG                                                                              \
	!SimpleLogStream::GetInstance()->verbose ? (void)0                             \
			: *SimpleLogStream::GetInstance() <                                    \
				SimpleLogStream::GetInstance()->_stream(vlog)
#define ILOG                                                                              \
	*SimpleLogStream::GetInstance() <                                              \
		SimpleLogStream::GetInstance()->_stream(ilog)
#define ELOG                                                                              \
	*SimpleLogStream::GetInstance() <                                              \
		SimpleLogStream::GetInstance()->_stream(elog)
// clang-format on

// helpers
#define MYNAME abi::__cxa_demangle(typeid(this).name(), 0, 0, nullptr) << ": "
#define ENTRY_LOGGER EntryLogger autoEntryLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__);

#ifdef NDEBUG
#define DLOG true ? (void)0 : SilentNoop() & ILOG
#else // DEBUG
#define DLOG                                                                                       \
	*SimpleLogStream::GetInstance() <                                                       \
		SimpleLogStream::GetInstance()->_stream(ilog)
#endif // NDEBUG

