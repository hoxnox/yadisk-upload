// Google Testing Framework
#include <gtest/gtest.h>

std::stringstream inflog;
std::stringstream errlog;
std::stringstream vrblog;

// test cases
#include <Logging.hpp>
#include "yandex/t_disk.hpp"
#include "yandex/t_tls_transport.hpp"


class DispatchToStream : public el::LogDispatchCallback
{
protected:
	void handle(const el::LogDispatchData *data) override
	{
		if (data->logMessage()->level() == el::Level::Fatal
		 || data->logMessage()->level() == el::Level::Error)
		{
			errlog << data->logMessage()->message() << std::endl;
		}
		else if (data->logMessage()->level() == el::Level::Trace)
		{
			vrblog << data->logMessage()->message() << std::endl;
		}
		else
		{
			inflog << data->logMessage()->message() << std::endl;
		}
	}
};

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	init_logging(1);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, "FALSE");
	el::Helpers::installLogDispatchCallback<DispatchToStream>("test");
	return RUN_ALL_TESTS();
}


