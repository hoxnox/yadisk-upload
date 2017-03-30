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

#include <utility>
#include <vector>

#include <yandex/transport.hpp>

class transport_mock : public yandex::transport
{
public:
	transport_mock() : transport() {}
	~transport_mock() {}

	result_t
	get(std::string url, response_handler_t handler = nullptr) override
	{
		cmd_.emplace_back(methods::GET, url);
		if (handler)
			handler(url, NULL, 0);
		return {transport::result_t::SUCCESS};
	}

	result_t
	put(std::string url,
	    std::basic_istream<char>& body,
	    size_t bodysz = 0,
	    response_handler_t handler = nullptr) override
	{
		if (url == "/v1/disk/resources/upload?path=bad")
		{
			if (handler)
				handler(url, NULL, 0);
			return {transport::result_t::FAILED};
		}
		cmd_.emplace_back(methods::PUT, url);
		if (handler)
			handler(url, NULL, 0);
		return {transport::result_t::SUCCESS};
	}

	void cancel(uint16_t code = 0, std::string message = "")
	{
	}

	std::shared_ptr<transport>
		make_transport(std::string host, uint16_t port) override
			{ return std::make_shared<transport_mock>(); }

	enum class methods : uint8_t
	{
		GET,
		PUT
	};

	std::vector<std::pair<methods, std::string>> cmd_;
private:
	std::string token_;
};

