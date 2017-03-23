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
#include <functional>
#include <vector>

namespace yandex {

class transport
{
public:
	using response_handler_t = std::function<void(std::vector<uint8_t>)>;

	transport(std::string token = "") : token_(token) {}
	virtual bool get(std::string url, response_handler_t handler = nullptr) = 0;
	virtual bool put(std::string url, response_handler_t handler = nullptr) = 0;

	virtual ~transport() {}

protected:
	std::string token_;
};

} // namespace

