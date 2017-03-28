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
#include "TlsEcho.hpp"

using namespace boost::asio;

void
TlsEcho::Start()
{
	VLOG << _("TlsEcho started.");
	start_notifier_.set_value();
	started_ = true;
	io_service_.run();
}

void
TlsEcho::Stop()
{
	VLOG << _("TlsEcho stopping.");
	io_service_.stop();
	VLOG << _("TlsEcho stopped.");
}

void 
TlsEcho::Dispatch()
{
	if(!thread_)
		thread_.reset(new std::thread([this](){Start();}));
	if (!started_)
	{
		auto waiter = start_notifier_.get_future();
		waiter.wait();
	}
}

TlsEcho::~TlsEcho()
{
	sock_->lowest_layer().shutdown(socket_base::shutdown_both);
	sock_->lowest_layer().close();
	Stop();
	if (thread_)
		thread_->join();
}

bool
TlsEcho::CAToFile(std::string filename)
{
	std::ofstream ofile(filename.c_str(), std::ios::binary);
	if (!ofile.is_open())
	{
		ELOG << _("TlsEcho: error creating file.")
		     << _(" Filename: ") << filename;
		return false;
	}
	std::ostreambuf_iterator<char> writer(ofile);
	std::copy(std::begin(DEFAULT_CA), std::end(DEFAULT_CERT), writer);
	return true;
}

TlsEcho::TlsEcho(std::string addr,
                 uint16_t port,
                 bool oneshot,
                 std::string certfile,
                 std::string privatefile,
                 std::string dhfile)
	: oneshot_(oneshot)
	, acceptor_(io_service_, ip::tcp::endpoint(ip::address::from_string(addr), port))
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

	sock_ = std::make_unique<ssl_socket>(io_service_, context_);

	acceptor_.async_accept(sock_->lowest_layer(),
		[this](boost::system::error_code err)
		{
			if (err)
			{
				ELOG << _("TlsEcho error.")
				     << _(" Message: ") << err.message();
				return;
			}
			VLOG << _("TlsEcho accepted. Starting handshake.");
			sock_->handshake(ssl::stream_base::server, err);
			if (err)
			{
				ELOG << _("TlsEcho handshake error.")
				     << _(" Message: ") << err.message();
				return;
			}
			std::vector<uint8_t> buf(1024);
			int rs = sock_->read_some(buffer(buf.data(), buf.capacity()), err);
			if (err)
			{
				ELOG << _("TlsEcho read_some error.")
				     << _(" Message: ") << err.message();
				return;
			}
			VLOG << _("TlsEcho have read data.") << _(" Size: ") << rs;
			rs = write(*sock_, buffer(buf.data(), rs), err);
			if (err)
			{
				ELOG << _("TlsEcho write error.")
				     << _(" Message: ") << err.message();
				return;
			}
			VLOG << _("TlsEcho have wrote data.") << _(" Size: ") << rs;
			if (oneshot_)
			{
				VLOG << _("TlsEcho oneshot finished. Stopping.");
				io_service_.stop();
				return;
			}
			VLOG << _("TlsEcho session finish. Waiting for the next.");
			acceptor_.accept(sock_->lowest_layer());
		});
}

const std::string TlsEcho::DEFAULT_KEY =
R"(-----BEGIN RSA PRIVATE KEY-----
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

const std::string TlsEcho::DEFAULT_CERT =
R"(-----BEGIN CERTIFICATE-----
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

const std::string TlsEcho::DEFAULT_DH =
R"(-----BEGIN DH PARAMETERS-----
MIIBCAKCAQEAjMjE/J9h71bciH4IfPXiSZb/aj5JV24/IWieb/Eyr5iKssjxPiKw
AHmp+3xQO8Y8D67D57+3pueob0sX9Oe0jj8janED+FubLJcik1o7nebFncCO0ubM
Z94B1O7bNo7299KbCFPQHTplWl8TW6CnL6/Q7LM4tarev+uuZudsKPVVpiG8SsTQ
dqYAomWZpPQGIH6px8A2vJd+iNn8iOMitAyXbPIfKq5BPWc73RTeSV7thTxxM1VZ
BuCq/8KZAzkIDF8XdzFq24/7BNRSJgEWW/atiCZMzJY0eNEMdTzLxHl/Vy+i+6+g
s+GGs3Eo45gVhEqVEUhVqclUyeYE9UdHcwIBAg==
-----END DH PARAMETERS-----
)";

const std::string TlsEcho::DEFAULT_CA =
R"(-----BEGIN CERTIFICATE-----
MIIFJTCCBA2gAwIBAgIBATANBgkqhkiG9w0BAQsFADCBnTELMAkGA1UEBhMCVVMx
CzAJBgNVBAgTAkNBMRUwEwYDVQQHEwxTYW5GcmFuY2lzY28xDjAMBgNVBAoTBVVG
T1JJMRYwFAYDVQQLEw1JVCBEZXBhcnRtZW50MREwDwYDVQQDEwhVRk9SSSBDQTEP
MA0GA1UEKRMGYWNjZXNzMR4wHAYJKoZIhvcNAQkBFg9hZG1pbkB1Zm9yaS5vcmcw
HhcNMTYwOTEzMTE0OTAyWhcNMjYwOTExMTE0OTAyWjCBmzELMAkGA1UEBhMCVVMx
CzAJBgNVBAgTAkNBMRUwEwYDVQQHEwxTYW5GcmFuY2lzY28xDjAMBgNVBAoTBVVG
T1JJMRYwFAYDVQQLEw1JVCBEZXBhcnRtZW50MQ8wDQYDVQQDEwZzZXJ2ZXIxDzAN
BgNVBCkTBmFjY2VzczEeMBwGCSqGSIb3DQEJARYPYWRtaW5AdWZvcmkub3JnMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvcG5psUN7X6mjO0LS2Rlohl6
Lx2CvHrtphAmTAlJ3VKUlWb+n5aBOzdpy5ytgc3ie+tExxRc1AB7ogCnQFpOMlDI
JWTbKt2BOkUddDYhYzEw4Cv172G2C+ERync4WTae5mJLNaV+aZMJBgj5khUjw7vd
2pVxsNdx6hspy1WzX+shv5llinMiSQL2PBpe7soAxLUsXV99bOTfcVuEVDFK9MZj
ScdkaIzxyYUnAwd0MirAh4+Ujh1439nwr1gGYILIwZiN6OqLpj8w2nW7Z/c3T2di
baDWEuCPTPyXFtI9YIdWCo/RYkLcrqJRcGQOtaU8y4QqAGqlj3OShrU3SXmdzwID
AQABo4IBbjCCAWowCQYDVR0TBAIwADARBglghkgBhvhCAQEEBAMCBkAwNAYJYIZI
AYb4QgENBCcWJUVhc3ktUlNBIEdlbmVyYXRlZCBTZXJ2ZXIgQ2VydGlmaWNhdGUw
HQYDVR0OBBYEFNyjGEudQb07b4RPR2Hf99szS9MCMIHSBgNVHSMEgcowgceAFP9Z
0XhAIUR+z3K/OtZ8BQERbQAnoYGjpIGgMIGdMQswCQYDVQQGEwJVUzELMAkGA1UE
CBMCQ0ExFTATBgNVBAcTDFNhbkZyYW5jaXNjbzEOMAwGA1UEChMFVUZPUkkxFjAU
BgNVBAsTDUlUIERlcGFydG1lbnQxETAPBgNVBAMTCFVGT1JJIENBMQ8wDQYDVQQp
EwZhY2Nlc3MxHjAcBgkqhkiG9w0BCQEWD2FkbWluQHVmb3JpLm9yZ4IJAI1sr6yA
SsEdMBMGA1UdJQQMMAoGCCsGAQUFBwMBMAsGA1UdDwQEAwIFoDANBgkqhkiG9w0B
AQsFAAOCAQEAHXOrIx4RNrWCaX0u0vrL3kE69hhN79/AJRrD0AB1cZoNO6f1ld1K
QklY+bfUm3Q9jgtoa75Cu+vDrSu8G2rgHnNkLpBjehOFVmDSSfyRamnApdP05Bal
hFFEZds7gkK+Lb/l3Wp5Q4ujvDxOjs5b3R7+qMuK6JN4hLHdexfLNFTXzXUjoHLS
j2s7/HgbclLPTMMMIBi5QVkYIWunjvtpqwcR2y+nSjfMt5m802l09iEC+4pgt9Gr
GctKeLRen41Kjh6iE1XDiq0v6RvkPxytu0HAooZU6pWjGDGvxBazpB/w/r+TvTsz
ne3i0CXdxOCdhEyRsJ4kJ8Oo2k5kL/z9mw==
-----END CERTIFICATE-----
)";
