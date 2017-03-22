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

#include <iostream>
#include <docopt/docopt.h>
#include "Logging.hpp"

static const char USAGE[] =
R"(Yandex disk uploader

Usage:
  yadisk-upload [-v] FILE
  yadisk-upload -h | --version

Arguments:
  FILE to upload

Options:
  -v --verbose   Make a lot of noise
  -h --help      Show help message
  --version      Show version
)";

struct Config
{
	void
	ParseArgs(int argc, char* argv[])
	{
		std::map<std::string, docopt::value> args
			= docopt::docopt(USAGE, { argv + 1, argv + argc }, true, VERSION);
		for (auto const& arg : args)
		{
			if (arg.second)
			{
				if (arg.first == "--verbose")
				{
					if (arg.second.asBool())
						verbose = true;
				}
			}
		}
	}
	bool verbose{false};
};

int
main(int argc, char* argv[])
{
	Config cfg;
	cfg.ParseArgs(argc, argv);
	init_logging(cfg.verbose ? 1 : 0);

	return 0;
}

