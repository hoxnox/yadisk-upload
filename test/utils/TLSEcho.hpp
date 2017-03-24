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

#pragma once

#include <cstdint>
#include <vector>
#include <chrono>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "BaseEcho.hpp"

class TLSEcho : public BaseEcho
{
public:
	TLSEcho(bool oneshot = true,
	        std::string certfile = "",
	        std::string privatefile = "",
	        std::string dhfile = "");

protected:
	void loop(std::string addr, unsigned short port) override;

private:
	bool oneshot_{true};
	static const std::string DEFAULT_CERT;
	static const std::string DEFAULT_KEY;
	static const std::string DEFAULT_DH;
	boost::asio::ssl::context context_{boost::asio::ssl::context::sslv23};
};

