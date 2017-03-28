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
#include <thread>
#include <future>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class TlsEcho
{
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
public:
	TlsEcho(std::string addr,
	        uint16_t port,
	        bool oneshot = true,
	        std::string certfile = "",
	        std::string privatefile = "",
	        std::string dhfile = "");
	~TlsEcho();
	void Start();
	void Stop();
	void Dispatch();
	bool CAToFile(std::string filename);

private:
	bool started_{false};
	std::promise<void> start_notifier_;
	std::unique_ptr<std::thread> thread_;
	bool oneshot_{true};
	static const std::string DEFAULT_KEY;
	static const std::string DEFAULT_CERT;
	static const std::string DEFAULT_DH;
	static const std::string DEFAULT_CA;
	boost::asio::ssl::context context_{boost::asio::ssl::context::sslv23};
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	std::unique_ptr<ssl_socket> sock_{nullptr};
};

