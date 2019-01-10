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
#include <logging.hpp>
#include <yandex/disk.hpp>

static const char USAGE[] = R"(Yandex disk uploader

Usage:
  yadisk-upload [-v] [-C] [-a TOKEN] [-f] SOURCE DEST
  yadisk-upload -h | --version

Arguments:
  SOURCE       file to upload
  DESTINATION  destination directory path

Options:
  -a TOKEN --auth=TOKEN  Authorization token
  -C --ssl-no-check      Don't check SSL certificate
  -f --force             Force overwrite on conflict
  -v --verbose           Make a lot of noise
  -h --help              Show help message
  --version              Show version

Examples:
  yadisk-upload -a 0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f ~/my/file remote/dir
)";

struct Config
{
	void ParseArgs(int argc, char *argv[])
	{
		std::map<std::string, docopt::value> args =
		    docopt::docopt(USAGE, {argv + 1, argv + argc}, true, VERSION);
		for (auto const &arg : args)
		{
			if (arg.second)
			{
				if (arg.first == "--verbose" && arg.second.asBool())
					verbose = true;
				if (arg.first == "--force" && arg.second.asBool())
					force = true;
				else if (arg.first == "--auth")
					auth = arg.second.asString();
				else if (arg.first == "--ssl-no-check")
					check_cert = false;
				else if (arg.first == "SOURCE")
					source = arg.second.asString();
				else if (arg.first == "DEST")
					dest = arg.second.asString();
			}
		}
	}
	bool verbose{false};
	std::string auth;
	std::string source;
	std::string dest;
	bool force{false};
	bool check_cert{true};
};

int
main(int argc, char *argv[])
{
	Config cfg;
	cfg.ParseArgs(argc, argv);
	init_logging(cfg.verbose ? 1 : 0);

	yandex::disk::api disk_api(cfg.auth, cfg.check_cert);
	if (!disk_api.upload(cfg.source, cfg.dest, cfg.force))
		return 1;

	return 0;
}
