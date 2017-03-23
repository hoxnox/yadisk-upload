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

#include <errno.h>
#include <cstring>

#include "tls_context.hpp"

namespace yandex
{

tls_context::tls_context(int sock, method_t m, std::string log_prefix,
		std::vector<uint8_t> cert, std::vector<uint8_t> pkey)
	: sock_(sock)
	, meth_(m)
{
	if (!log_prefix.empty())
	{
		log_prefix_ += "<";
		log_prefix_ += log_prefix;
		log_prefix_ += "> ";
	}

	SSL_METHOD* meth;
	switch (meth_)
	{
		case method_t::SRV:
			meth = (SSL_METHOD*)TLS_server_method();
			break;
		case method_t::CLI:
		default:
			meth = (SSL_METHOD*)TLS_client_method();
			break;
	}
	ssl_ctx_.reset(SSL_CTX_new(meth));
	if (!ssl_ctx_)
	{
		ELOG << log_prefix_ << _("Creation of a new SSL_CTX object failed.")
		     << (" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		return;
	}
	if (!cert.empty() && SSL_CTX_use_certificate_ASN1(ssl_ctx_.get(),
				cert.size(), (uint8_t*)cert.data()) != 1)
	{
		ELOG << log_prefix_ << _("Error loading certificate.")
		     << _(" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		return;
	}
	if (!pkey.empty() && SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, ssl_ctx_.get(),
				(uint8_t*)pkey.data(), pkey.size()) != 1)
	{
		ELOG << log_prefix_ << _("SSL_CTX_use_PrivateKey_file.")
		     << _(" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		return;
	}
	SSL_CTX_set_verify(ssl_ctx_.get(), SSL_VERIFY_PEER, dontVerify);
	SSL_CTX_set_verify_depth(ssl_ctx_.get(), 1);

	sbio_.reset(BIO_new_socket(sock, BIO_NOCLOSE));
	if (!sbio_)
	{
		ELOG << log_prefix_ << _("The creation of a new BIO structure failed")
		     << (" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		return;
	}
	BIO_set_nbio(sbio_.get(), 1);

	ssl_.reset(SSL_new(ssl_ctx_.get()));
	if(!ssl_)
	{
		ELOG << log_prefix_ << _("The creation of a new SSL structure failed")
		     << (" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		BIO_free(sbio_.get());
		sbio_.reset(nullptr);
		return;
	}
	SSL_set_bio(ssl_.get(), sbio_.get(), sbio_.get());
	if (SSL_set_cipher_list(ssl_.get(), "SSLv2:SSLv3:TLSv1:TLSv1.1:TLSv1.2:SHA1") == 0)
	{
		ELOG << log_prefix_ << _("SSL_set_cipher_list falied.")
		     << (" Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
		ssl_.reset(nullptr);
		return;
	}
	switch (meth_)
	{
		case method_t::SRV:
			SSL_set_accept_state(ssl_.get());
			break;
		case method_t::CLI:
		default:
			SSL_set_connect_state(ssl_.get());
			break;
	}
	VLOG << log_prefix_ << _("TlsContext initialized.");
}

tls_context::~tls_context()
{
	if (ssl_)
		SSL_shutdown(ssl_.get());
	VLOG << log_prefix_ << _("TlsContext destroyed.");
}

int
tls_context::start()
{
	if (sock_ == -1 || !ssl_)
	{
		ELOG << log_prefix_ << _("Context is not initialized.");
		return -1;
	}

	int rs = 0;
	if (meth_ == method_t::SRV)
	{
		rs = SSL_accept(ssl_.get());
	}
	else if (meth_ == method_t::CLI)
	{
		rs = SSL_connect(ssl_.get());
	}
	else
	{
		VLOG << log_prefix_ << _("Unknown TLS method. Making SSL_connect.");
		rs = SSL_connect(ssl_.get());
	}

	if (rs != 1)
	{
		int code = SSL_get_error(ssl_.get(), rs);
		if (code == SSL_ERROR_WANT_READ)
			return 1;
		if (code == SSL_ERROR_WANT_WRITE)
			return 2;
		VLOG << log_prefix_ << _(" SSL_connect falied.")
		     << (" Message: ") << ERR_error_string(code, nullptr);
		return 3;
	}

	return 0;
}

} // namespace

