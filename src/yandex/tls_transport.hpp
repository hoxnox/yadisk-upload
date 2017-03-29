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

namespace yandex {

class tls_transport : public transport
{
public:
	tls_transport(std::string token = "",
	              std::string host = "cloud.yandex.net",
	              uint16_t port = 443,
	              bool dont_verify = false);
	~tls_transport();
	/**@brief perform HTTP GET request*/
	op_results get(std::string url, response_handler_t handler = nullptr) override;

	/**@brief perform HTTP PUT request*/
	op_results put(std::string url,
	               std::basic_istream<char>& body,
	               size_t bodysz = 0,
	               response_handler_t handler = nullptr) override;
	void cancel() override;

private:
	class tls_transport_impl_;
	tls_transport_impl_* impl_{nullptr};
};

} // namespace


