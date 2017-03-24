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

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include <boost/bind.hpp>

#include <logging.hpp>
#include "TLSEcho.hpp"

using namespace boost::asio;
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

TLSEcho::TLSEcho(bool oneshot,
                 std::string certfile,
                 std::string privatefile,
                 std::string dhfile)
	: BaseEcho()
{
	context_.set_options(
		ssl::context::default_workarounds
		| ssl::context::no_sslv2
		| ssl::context::single_dh_use);

	if (!certfile.empty())
		context_.use_certificate_chain_file(certfile);
	else
		context_.use_certificate(const_buffer(DEFAULT_CERT.c_str(), DEFAULT_CERT.length()), ssl::context::pem);

	if (!privatefile.empty())
		context_.use_private_key_file(privatefile, ssl::context::pem);
	else
		context_.use_private_key(const_buffer(DEFAULT_KEY.c_str(), DEFAULT_KEY.length()), ssl::context::pem);

	if (!dhfile.empty())
		context_.use_tmp_dh_file(dhfile.c_str());
	else
		context_.use_tmp_dh(const_buffer(DEFAULT_DH.c_str(), DEFAULT_DH.length()));
}

void
TLSEcho::loop(std::string saddr, unsigned short port)
{
	io_service   io_service;
	ip::tcp::acceptor acc(io_service, ip::tcp::endpoint(
		ip::address::from_string(saddr), port));
	ssl_socket sock(io_service, context_);
	acc.async_accept(sock.lowest_layer(),
		[&](boost::system::error_code err)
		{
			if (err)
			{
				ELOG << _("TLSEcho error.")
				     << _(" Message: ") << err.message();
				return;
			}
			sock.handshake(ssl::stream_base::server, err);
			if (err)
			{
				ELOG << _("TLSEcho handshake error.")
				     << _(" Message: ") << err.message();
				return;
			}
			std::vector<uint8_t> buf(1024);
			int rs = sock.read_some(buffer(buf.data(), buf.capacity()), err);
			if (err)
			{
				ELOG << _("TLSEcho read_some error.")
				     << _(" Message: ") << err.message();
				return;
			}
			rs = write(sock, buffer(buf.data(), rs), err);
			if (err)
			{
				ELOG << _("TLSEcho read_some error.")
				     << _(" Message: ") << err.message();
				return;
			}
			acc.accept(sock.lowest_layer());
		});
	io_service.run();
}

const std::string TLSEcho::DEFAULT_CERT =
R"(
-----BEGIN RSA PRIVATE KEY-----
MIICXgIBAAKBgQDm9g893+HAbZwBclgefhFU29UJCQ6WqTDAqZiE+Sw8aW2jz/Ov
yktRctNYSVfAiyxZ7uW07XZfPneDA6k/6OWXkUE8fWnRGROJ9Dsoj3wwjt+ta9Gl
PU8mjYN81W0i9BanOzpnHPd1+m/nhZYfeOs/Fm0nLNWqM2ficCtndU4LXQIDAQAB
AoGAL9GoTDZLZm0LTN1g00dkzT4KuKkwZQ84sdsrYfS5LPTjJ3SJzs7Ck2WDzpa4
3XuzPheRZcl4pbWoRu7+HuZYiolTFw3QQeTbxV4246iZ95a/+mOFk0JIz9ZuieTP
hAlyKHE4axRjVcwl08S/7hHFjFCoZre0ttz455AfB7a5L/0CQQD+tI3MhC2LRAqK
ZfGci/9zpeY7sO1UJtkGlMA+bp2ZM8t6VGVu05+T+BczeQhYF1oU6drLwMkWjrxs
xppAO8TzAkEA6CKbixUmfEozjSVrLl/48i4nV17xwho1DXZ01XQecFVv3hZuaWbA
68k5c/wYn9NYhZV/WsWhERR0bA3ZTi9CbwJBAN1EMu8ZYItcZ5/FYfiyMg/gbjsq
v3Hccari7IMQCz79HOY/jQKTO00LN/SST2pflvUWFunsp4Q+KXiBq5zjj3sCQQCA
nmxAa7+fLh3SHuF8GID+7sOtjVW2jn7GrtOdUXFsHGqXSyZyEBTkj+HdedjK9Xce
zO57hPTxWbwIAsRcpaLRAkEAnnzj5b6iqt5ck693tOQ4kWK4ZqpcaA3AuFSqdbwA
5XNYwIop/2EZjG4bdwfG5SNobuBDCwUReyvj/U9LzVKFRA==
-----END RSA PRIVATE KEY-----
)";

const std::string TLSEcho::DEFAULT_KEY =
R"(
-----BEGIN CERTIFICATE-----
MIICxTCCAi6gAwIBAgIBAjANBgkqhkiG9w0BAQsFADCBqTELMAkGA1UEBhMCVVMx
EzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDVNhbnRhIEJhcmJhcmExEzAR
BgNVBAoMClNTTCBTZXJ2ZXIxIjAgBgNVBAsMGUZvciBUZXN0aW5nIFB1cnBvc2Vz
IE9ubHkxFTATBgNVBAMMDGxvY2FsaG9zdCBDQTEdMBsGCSqGSIb3DQEJARYOcm9v
dEBsb2NhbGhvc3QwHhcNMTYwOTEzMTE0MDM0WhcNMTgwOTEzMTE0MDM0WjCBpjEL
MAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDVNhbnRh
IEJhcmJhcmExEzARBgNVBAoMClNTTCBTZXJ2ZXIxIjAgBgNVBAsMGUZvciBUZXN0
aW5nIFB1cnBvc2VzIE9ubHkxEjAQBgNVBAMMCWxvY2FsaG9zdDEdMBsGCSqGSIb3
DQEJARYOcm9vdEBsb2NhbGhvc3QwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGB
AOb2Dz3f4cBtnAFyWB5+EVTb1QkJDpapMMCpmIT5LDxpbaPP86/KS1Fy01hJV8CL
LFnu5bTtdl8+d4MDqT/o5ZeRQTx9adEZE4n0OyiPfDCO361r0aU9TyaNg3zVbSL0
Fqc7Omcc93X6b+eFlh946z8WbScs1aozZ+JwK2d1TgtdAgMBAAEwDQYJKoZIhvcN
AQELBQADgYEAizioDhQq/V/4HxO+2Dtx/ngL8zwhaR7JA6or01dfL/wqEuj43+q7
sIQdDZxsa++/fXUOwSLPlkAGFXfrlC+Y9Ut4hFWDYGfUwe42Ji0XRy/CVuupN+k+
RmYalUhzokNTc1/r52bRrUmkDMZwvD8ujx/E2a6MzQkTv6+uiFobAsc=
-----END CERTIFICATE-----
)";

const std::string TLSEcho::DEFAULT_DH =
R"(
-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEAjMjE/J9h71bciH4IfPXiSZb/aj5JV24/IWieb/Eyr5iKssjxPiKw
AHmp+3xQO8Y8D67D57+3pueob0sX9Oe0jj8janED+FubLJcik1o7nebFncCO0ubM
Z94B1O7bNo7299KbCFPQHTplWl8TW6CnL6/Q7LM4tarev+uuZudsKPVVpiG8SsTQ
dqYAomWZpPQGIH6px8A2vJd+iNn8iOMitAyXbPIfKq5BPWc73RTeSV7thTxxM1VZ
BuCq/8KZAzkIDF8XdzFq24/7BNRSJgEWW/atiCZMzJY0eNEMdTzLxHl/Vy+i+6+g
s+GGs3Eo45gVhEqVEUhVqclUyeYE9UdHcwIBAg==
-----END DH PARAMETERS-----
)";

