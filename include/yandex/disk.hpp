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

#include <memory>

#include "transport.hpp"

namespace yandex {
namespace disk {

class api
{
public:
	api(std::string token);
	api(std::shared_ptr<transport> cmd_transport);
	~api();

	/**@brief Upload file source into directory destination
	 * @param destination directory on disk to upload
	 * @param source file path on local machine
	 * @note directories should exists*/
	bool upload(std::string source, std::string destination);

private:
	struct api_impl_;
	api_impl_* impl_;
};


}} // namespace
