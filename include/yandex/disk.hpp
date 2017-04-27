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
#include <memory>

#include "transport.hpp"

namespace yandex {
namespace disk {

class api
{
public:
	api(std::string token, bool cert_check = true);
	api(std::shared_ptr<transport> cmd_transport);
	~api();

	/**@brief Upload file source into destination
	 * @param destination file name on disk
	 * @param source file path on local machine
	 * @param chunksz - size of chunk in chunk-encoding (default to 1024*1024)
	 * @note directories should exists*/
	bool upload(std::string source, std::string destination, size_t chunksz = 0);
	/**@brief Uploads data from input stream.
	 * @param size - how many bytes to read from the stream. In case of
	 * @param chunksz - size of chunk in chunk-encoding (default to 1024*1024)
	 * zero - read while strm->good()*/
	bool upload(std::string destination, std::istream& strm, size_t datasz = 0, size_t chunksz = 0);

private:
	struct api_impl_;
	api_impl_* impl_;
};


}} // namespace
