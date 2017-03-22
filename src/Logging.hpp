#pragma once

#include <easylogging++.h>
#include <gettext.h>

void inline
init_logging(int level)
{
	el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
	el::Loggers::setLoggingLevel(el::Level::Fatal);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format,
			"%levshort%datetime{%Y%M%dT%H%m%s} %msg");
	if (level > 0)
		el::Loggers::setLoggingLevel(el::Level::Trace);
}

#ifdef ILOG
#undef ILOG
#endif
#define ILOG LOG(INFO)

#ifdef ELOG
#undef ELOG
#endif
#define ELOG LOG(ERROR)

#ifdef VLOG
#undef VLOG
#endif
#define VLOG LOG(TRACE)
