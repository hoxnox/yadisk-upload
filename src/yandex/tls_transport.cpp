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

#include "tls_transport.hpp"
#include <yandex/logging.hpp>

namespace yandex {

tls_transport::tls_transport(std::string token)
	: transport(token)
{
	ILOG << "ctor";
}

bool
tls_transport::get(std::string url, response_handler_t handler)
{
	ILOG << "get";
}

bool
tls_transport::put(std::string url, response_handler_t handler)
{
	ILOG << "put";
}

} // namespace

