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

class url_t;

struct api::api_impl_
{
	url_t get_upload_url(const std::string& destination);
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

	operator bool() { return !host.empty(); }

	std::string host;
	std::string suffix;
	uint16_t port;
};

template<class InIter> inline std::string
bytes2str(InIter begin, InIter end)
{
	std::string rs;
	while (begin != end)
		rs += hex(*begin++);
	return rs;
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
find_response_body(InIter begin, InIter end)
{
	const uint8_t body_end[] = {'\r', '\n', '\r', '\n'};
	InIter rs = std::search(begin, end, body_end, body_end + sizeof(body_end));
	if (rs == end)
		return end;
	return rs + 4;
}

inline std::string
fetch_upload_url(std::string str)
{
	auto pos = str.begin();
	auto end = str.end();
	if ((pos = find_response_body(pos, end)) == end)
		return {};
	std::string link_prefix = "\"href\":\"";
	if ((pos = std::search(pos, end, link_prefix.begin(), link_prefix.end())) == end)
		return {};
	pos += link_prefix.length();
	auto href_end = std::find(pos, end, '\"');
	if (href_end == end)
		return {};
	 return {pos, href_end};
}

url_t
api::api_impl_::get_upload_url(const std::string& destination)
{
	std::string raw_response;
	std::string url = "/v1/disk/resources/upload?path=" + url_encode(destination);
	auto rs = cmd_transport->get("/v1/disk/resources/upload?path=" + url_encode(destination),
			[&raw_response, &url](const std::string& url_, const uint8_t* data, size_t datasz)
			{
				if (url == url_)
					raw_response += std::string(data, data + datasz);
			});
	if (!rs || raw_response.empty())
	{
		ELOG << _("Error receiving GET response. ") << rs;
		return {};
	}
	VLOG << _("Successful executed GET request for put data.")
	     << _(" Raw: ") << raw_response;
	std::string upload_url = fetch_upload_url(raw_response);
	if (upload_url.empty())
	{
		ELOG << _("Error fetching upload URL.");
		return {};
	}
	VLOG << _("URL fetched. Parsing. ") << upload_url;
	auto parsed_upload_url = url_t::from_string(upload_url);
	if (!parsed_upload_url)
	{
		ELOG << _("Error parsing upload URL.")
		     << _(" URL: ") << upload_url;
		return {};
	}
	VLOG << ("Got upload url. ") << parsed_upload_url.str();
	return parsed_upload_url;
}

/**@note Don't escape special symbols in parameters. Use UTF-8.*/
bool
api::upload(std::string source, std::string destination, size_t chunksz)
{
	boost::filesystem::path fs_source(source);
	std::ifstream ifile(fs_source.string().c_str(), std::ios::binary);
	if (!ifile.good())
	{
		ELOG << _("Error opening file.")
		     << _(" Filename: \"") << fs_source.string() << "\"";
		return false;
	}
	return upload(destination, ifile, chunksz);
}

bool
api::upload(std::string destination, std::istream& strm, size_t size, size_t chunksz)
{
	auto url = impl_->get_upload_url(destination);
	if (!url)
		return false;

	auto put_transport = impl_->cmd_transport->make_transport(url.host, url.port, chunksz);
	auto rs = put_transport->put(url.suffix, strm, size);
	if (!rs)
	{
		ELOG << _("Error putting. ") << rs;
		return false;
	}
	return true;
}

}} // namespace

