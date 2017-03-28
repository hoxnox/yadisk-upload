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

#include <yandex/transport.hpp>

class transport_mock : public yandex::transport
{
public:
	transport_mock() : transport() {}
	~transport_mock() {}

	op_result_t get(std::string url, response_handler_t handler = nullptr) override
		{ return transport::op_result_t::FAILED; }

	op_result_t put(std::string url,
	                std::basic_istream<char>& body,
	                response_handler_t handler = nullptr) override
		{ return transport::op_result_t::FAILED; }

	void cancel() override {}

private:
	std::string token_;
};

