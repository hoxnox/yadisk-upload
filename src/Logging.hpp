/*
Copyright 2017 hoxnox <hoxnox@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __LOGGING_GUARD__
#define __LOGGING_GUARD__

#include <easylogging++.h>

#ifndef _
#define _(X) (X)
#endif

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

#endif // __LOGGING_GUARD__
