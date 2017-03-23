// Google Testing Framework
#include <gtest/gtest.h>

// test cases
#include <Logging.hpp>
#include "yandex/t_disk.hpp"
#include "yandex/t_tls_transport.hpp"

int main(int argc, char *argv[])
{
	init_logging(1);
	ILOG << "I";
	ELOG << "E";
	VLOG << "V";
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


