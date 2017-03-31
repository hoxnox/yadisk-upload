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

#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
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

inline
std::string hex(unsigned char c)
{
	std::string rs;
	const char hex[] = "0123456789ABCDEF";
	rs += hex[c/0x10];
	rs += hex[c%0x10];
	return rs;
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
			rs << '%' << hex(c);
	}
	return rs.str();
}

inline const uint8_t*
find_response_body(const uint8_t* begin, const uint8_t* end)
{
	const uint8_t body_end[] = {'\r', '\n', '\r', '\n'};
	const uint8_t* rs = std::search(begin, end, body_end, body_end + sizeof(body_end));
	if (rs == end)
		return end;
	return rs + 4;
}

template<class InIter> inline InIter
check_prefix(InIter begin, InIter end, std::string prefix)
{
	if (end < begin || end - begin < prefix.length())
		return end;
	if (std::equal(begin, begin + prefix.length(), prefix.data()))
		return begin + prefix.length();
	return end;
}


class url_t
{
public:
	url_t(std::string host = "", uint16_t port = 0, std::string suffix = "")
		: host(host), port(port), suffix(suffix)
	{ }
	static url_t from_string(std::string url)
	{
		std::string host, suffix;
		uint16_t port = 0;
		auto pos = url.begin();
		if ((pos = check_prefix(url.begin(), url.end(), "https://")) == url.end())
			return {};
		auto host_end_pos = std::find(pos, url.end(), '/');
		if (host_end_pos != url.end())
			suffix = std::string(host_end_pos, url.end());
		std::string host_port = std::string(pos, host_end_pos);
		size_t colon_pos = host_port.find(':');
		if (colon_pos)
		{
			host = std::string(pos, pos + colon_pos);
			try
			{
				port = boost::lexical_cast<uint16_t>(std::string(pos + colon_pos + 1, host_end_pos));
			}
			catch (boost::bad_lexical_cast& e)
			{
				return {};
			}
		}
		else
		{
			host = host_port;
		}
		return {host, port, suffix};
	}
	std::string str() const
	{
		std::stringstream ss;
		ss << _(" Host: ") << host
		   << _(", port: ") << port
		   << _(", suffix: ") << suffix;
		return ss.str();
	}

	operator bool() { return host.empty(); }

	std::string host;
	std::string suffix;
	uint16_t port;
};

/**@note Don't escape special symbols in parameters. Use UTF-8.*/
bool
api::upload(std::string source, std::string destination)
{
	boost::filesystem::path fs_source(source);
	std::ifstream ifile(fs_source.string().c_str(), std::ios::binary);
	if (!ifile.good())
	{
		ELOG << _("Error opening file.")
		     << _(" Filename: \"") << fs_source.string() << "\"";
		return false;
	}
	std::string upload_url;
	std::string url = "/v1/disk/resources/upload?path=" + url_encode(destination);
	auto rs = impl_->cmd_transport->get("/v1/disk/resources/upload?path=" + url_encode(destination),
			[&upload_url, &url](const std::string& url_, const uint8_t* pos, size_t datasz)
			{
				if (!upload_url.empty() || url != url_)
					return;
				const uint8_t* end = pos + datasz;
				if ((pos = find_response_body(pos, end)) == end)
					return;
				if ((pos = check_prefix(pos, end, "{\"href\":\"")) == end)
					return;
				const uint8_t* href_end = std::find(pos, end, '\"');
				if (href_end == end)
					return;
				upload_url = std::string(pos, href_end);
			});
	if (!rs || upload_url.empty())
	{
		ELOG << _("Error getting upload URL. ") << rs;
		return false;
	}
	auto parsed_upload_url = url_t::from_string(upload_url);
	if (parsed_upload_url)
	{
		ELOG << _("Error parsing upload URL.")
		     << _(" URL: ") << upload_url;
		return false;
	}
	VLOG << ("Got upload url. ") << parsed_upload_url.str();

	auto put_transport = impl_->cmd_transport->make_transport(
			parsed_upload_url.host, parsed_upload_url.port);
	rs = put_transport->put(parsed_upload_url.suffix, ifile);
	if (!rs)
	{
		ELOG << _("Error putting. ") << rs;
		return false;
	}
	return true;
}

}} // namespace

