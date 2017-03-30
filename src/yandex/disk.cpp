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

#include <iomanip>
#include <sstream>

#include <yandex/disk.hpp>
#include <logging.hpp>

#include "tls_transport.hpp"

namespace yandex {
namespace disk {

struct api::api_impl_
{
	std::shared_ptr<transport> cmd_transport{nullptr};
};

api::api(std::string token)
	: impl_(new api_impl_)
{
	impl_->cmd_transport = std::make_shared<tls_transport>(token);
}

api::api(std::shared_ptr<transport> transport)
	: impl_(new api_impl_)
{
	impl_->cmd_transport = transport;
}

api::~api()
{
	if (impl_)
		delete impl_;
}

inline std::string
url_encode(std::string str)
{
	std::stringstream rs;
	for (auto c : str)
	{
		if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') 
			rs << c;
		else if (c == ' ') 
			rs << '+';
		else 
			rs << '%' << std::setw(2) << std::setfill('0') << std::hex << (int)c;
	}
	return rs.str();
}

/**@note Don't escape special symbols in parameters. Use UTF-8.*/
bool
api::upload(std::string source, std::string destination)
{
	auto rs = impl_->cmd_transport->get("/v1/disk/resources/upload?path=" + url_encode(destination));
	if (!rs)
	{
		ELOG << _("Error getting upload URL.")
		     << _(" Code: ") << rs.code() << _(", Message: ") << rs.message();
		return false;
	}
}

}} // namespace

