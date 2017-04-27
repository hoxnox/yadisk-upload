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

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>
#include <iomanip>

#include "tls_transport.hpp"
#include <logging.hpp>

namespace yandex {

using namespace boost::asio;
using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
using ssl_socket = ssl::stream<tcp::socket>;

enum class states : uint8_t
{
	NONE,
	READY,
	INPROGRESS
};

struct tls_transport::tls_transport_impl_
{
	io_service        srv;
	ssl::context      ctx{ssl::context::sslv23};
	ssl_socket        sock{srv, ctx};
	std::string       host;
	states            state{states::NONE};
	size_t            io_bufsz{1024*1024};
	std::vector<char> io_buf;
	transport::result_t read_response(std::string url, transport::response_handler_t handler);
	transport::result_t parse_status(const char* data, size_t datasz);
};


tls_transport::tls_transport(std::string token,
                             std::string host,
                             uint16_t port,
                             bool dont_verify,
                             size_t chunksz)
	: transport(token, host)
	, impl_(new tls_transport_impl_)
	, token_(token)
{
	if (chunksz != 0)
		impl_->io_bufsz = chunksz;
	impl_->host  = host;
	impl_->io_buf.reserve(impl_->io_bufsz);
	try
	{
		impl_->ctx.set_default_verify_paths();
		if (dont_verify)
		{
			impl_->sock.set_verify_mode(ssl::verify_none);
		}
		else
		{
			impl_->sock.set_verify_mode(ssl::verify_peer);
			impl_->sock.set_verify_callback(ssl::rfc2818_verification(host));
		}

		tcp::resolver resolver(impl_->srv);
		tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
		connect(impl_->sock.lowest_layer(), resolver.resolve(query));
		impl_->sock.lowest_layer().set_option(tcp::no_delay(true));
		impl_->sock.handshake(ssl_socket::client);
		VLOG << _("TLS handshake succeeded.");
		impl_->state = states::READY;
	}
	catch (std::exception& e)
	{
		ELOG << "asio exception: " << e.what() << "\n";
	}
}

tls_transport::~tls_transport()
{
	if (impl_)
	{
		boost::system::error_code err;
		impl_->sock.shutdown(err);
		delete impl_;
	}
}

transport::result_t
tls_transport::tls_transport_impl_::parse_status(const char* status, size_t totalsz)
{
	std::match_results<const char*> m;
	const char* status_end = status;
	while (status_end[0] != '\r' && status_end[1] != '\n' && status_end < status + totalsz)
		++status_end;
	std::regex rx_status = std::regex("^HTTP/1.[012]+\\s+(\\d\\d\\d)\\s*(.*)$", std::regex::icase);
	if (!std::regex_match(status, status_end, m, rx_status))
	{
		ELOG << _("Error parsing status line.")
		     << _(" Status: ") << std::string(status, status_end);
		return {transport::result_t::FAILED};
	}
	uint16_t code = boost::lexical_cast<uint16_t>(m[1]);
	if (code < 200 || code > 299)
		return {transport::result_t::FAILED, code, m[2]};
	return {transport::result_t::SUCCESS, code, m[2]};
}

transport::result_t
tls_transport::tls_transport_impl_::read_response(std::string url, transport::response_handler_t handler)
{
	streambuf buf(io_bufsz);
	boost::system::error_code err;
	int headers_sz = read_until(sock, buf, "\r\n\r\n", err);
	if (err)
	{
		ELOG << _("Error reading server response headers.")
		     << _(" Message: ") << err.message();
		return {transport::result_t::FAILED};
	}
	VLOG << _("Headers read.") << _(" Size: ") << headers_sz
	                           << _(" Buffer size: ") << buf.size();
	if (handler)
		handler(url, buffer_cast<const uint8_t*>(buf.data()), headers_sz);

	const char* headers_data = buffer_cast<const char*>(buf.data());
	transport::result_t op_rs = parse_status(headers_data, headers_sz);

	std::match_results<const char*> m;
	std::regex rx = std::regex("content-length:\\s*(\\d+)", std::regex::icase);
	if (std::regex_search(headers_data, headers_data + headers_sz, m, rx))
	{
		size_t content_length = boost::lexical_cast<size_t>(m[1]);
		VLOG << _("Server response has body.") << _(" Size: ") << content_length;
		if (content_length > io_bufsz)
		{
			VLOG << _("Server response body will be truncated to the length of io_buf.")
			     << _(" Content-Length: ")  << content_length << ", buffer size: " << io_bufsz;
			content_length = io_bufsz;
		}
		buf.consume(headers_sz);
		if (buf.size() > 0 && content_length > 0)
		{
			int to_consume = std::min(buf.size(), content_length);
			if (handler)
				handler(url, buffer_cast<const uint8_t*>(buf.data()), to_consume);
			buf.consume(to_consume);
			content_length -= to_consume;
		}
		if (content_length == 0)
			return op_rs;
		int body_sz = read(sock, buf, transfer_exactly(content_length), err);
		if (err)
		{
			ELOG << _("Error reading server response body.")
			     << _(" Message: ") << err.message();
			return {transport::result_t::FAILED};
		}
		if (handler)
			handler(url, buffer_cast<const uint8_t*>(buf.data()), body_sz);
		VLOG << _("Server response body read.") << _(" Total size: ") << headers_sz + body_sz;
	}

	return op_rs;
}

/**@brief perform HTTP GET request*/
transport::result_t
tls_transport::get(std::string url, response_handler_t handler)
{
	if (impl_->state == states::INPROGRESS)
	{
		VLOG << ("Attempt to call `get` with busy transport.");
		return {result_t::INPROGRESS};
	}
	else if (impl_->state != states::READY)
	{
		VLOG << ("Transport is not ready.");
		return {result_t::FAILED};
	}
	boost::system::error_code err;
	int rs;
	std::stringstream req;
	req << "GET " << url << " HTTP/1.1\r\n"
	    << "Host: " << impl_->host << "\r\n"
	    << "User-Agent: hoxnox/yadisk-upload\r\n"
	    << "Accept: */*\r\n"
	    << "Authorization: OAuth " << token_ << "\r\n\r\n";
	rs = write(impl_->sock, buffer(req.str().c_str(), req.str().length()), err);
	if (err)
	{
		ELOG << _("Error writing GET request.")
		     << _(" URL: ") << url
		     << _(" Message: ") << err.message();
		return {result_t::FAILED};
	}
	VLOG << _("Successfully send GET request.")
	     << _(" URL: ") << url
	     << _(" Bytes written: ") << rs;

	return impl_->read_response(url, handler);
}

/**@brief perform HTTP PUT request*/
transport::result_t
tls_transport::put(std::string url,
                   std::basic_istream<char>& body,
                   size_t bodysz,
                   response_handler_t handler)
{
	if (impl_->state == states::INPROGRESS)
	{
		VLOG << ("Attempt to call `get` with busy transport.");
		return result_t::INPROGRESS;
	}
	else if (impl_->state != states::READY)
	{
		VLOG << ("Transport is not ready.");
		return result_t::FAILED;
	}
	boost::system::error_code err;
	int rs;

	std::stringstream req;
	req << "PUT " << url << " HTTP/1.1\r\n"
	    << "Host: " << impl_->host << "\r\n"
	    << "User-Agent: hoxnox/yadisk-upload\r\n"
	    << "Accept: */*\r\n"
	    << "Connection: keep-alive\r\n"
	    << "Transfer-Encoding: chunked\r\n"
	    << "Content-Type: application/octet-stream\r\n";

	VLOG << "Sending request.\n" << req.str();
	rs = write(impl_->sock, buffer(req.str().c_str(), req.str().length()), err);
	if (err)
	{
		ELOG << _("Error writing PUT request.")
		     << _(" URL: ") << url
		     << _(" Message: ") << err.message();
		return result_t::FAILED;
	}
	VLOG << _("Successfully send PUT headers.")
	     << _(" URL: ") << url
	     << _(" Bytes written: ") << rs;

	// send data
	size_t data_read = 0;
	while (body.good() && (bodysz == 0 || data_read < bodysz))
	{
		body.read(impl_->io_buf.data(), impl_->io_bufsz);
		if (body.gcount() == 0)
			break;
		data_read += body.gcount();
		std::stringstream length;
		length << "\r\n" << std::hex << body.gcount() << "\r\n";
		rs = write(impl_->sock, buffer(length.str().c_str(), length.str().length()), err);
		if (err || rs != length.str().length())
		{
			ELOG << _("Error putting chunk length.")
				 << _(" rs: ") << rs
				 << _(" attempt to send: ") << length.str().length()
			     << _(" URL: ") << url
			     << _(" Message: ") << err.message();
			return result_t::FAILED;
		}
		rs = write(impl_->sock, buffer(impl_->io_buf.data(), body.gcount()), err);
		if (err || rs != body.gcount())
		{
			ELOG << _("Error putting data chunk.")
				 << _(" rs: ") << rs
				 << _(" attempt to send: ") << body.gcount()
			     << _(" URL: ") << url
			     << _(" Message: ") << err.message();
			return result_t::FAILED;
		}
		VLOG << _("Sent PUT data portion.")
		     << _(" Size: ") << rs;
	}
	const std::string end_of_stream("\r\n0\r\n\r\n");
	rs = write(impl_->sock, buffer(end_of_stream.c_str(), end_of_stream.length()), err);
	if (err || rs != end_of_stream.length())
	{
		ELOG << _("Error putting chunk length.")
			 << _(" rs: ") << rs
			 << _(" attempt to send: ") << body.gcount()
		     << _(" URL: ") << url
		     << _(" Message: ") << err.message();
		return result_t::FAILED;
	}

	return impl_->read_response(url, handler);
}

void
tls_transport::cancel(uint16_t code, std::string message)
{
}

} // namespace

