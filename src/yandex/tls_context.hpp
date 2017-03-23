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
#include <string>
#include <memory>
#include <vector>
#include <cstdint>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/asn1.h>
#include <openssl/err.h>

#include <logging.hpp>

namespace yandex {

class tls_context
{
public:

	enum class method_t : uint8_t
	{
		UNKNOWN  = 1,
		CLI      = 3, // non-even for CLIENTS
		SRV      = 4
	};

	tls_context(int sock,
	            method_t meth,
	            std::string log_prefix = "",
	            std::vector<uint8_t> cert = {},
	            std::vector<uint8_t> pkey = {});
	~tls_context();

	/**@brief Start communication with the object.
	 * @return  0 - TLS communication process finished.
	 *          1 - WANT_READ
	 *          2 - WANT_WRITE
	 *          <0 - unrecoverable error
	 *          >2 - change object
	 *
	 * Method based. SRV - TCP connect and wait for ClientHello. CLI -
	 * TCP connect and send ClientHello.*/
	int start();

	SSL* ssl() const { return ssl_.get(); }

private:

	tls_context(const tls_context&) = delete;
	tls_context& operator=(const tls_context&) = delete;

	static int dontVerify(int ok, X509_STORE_CTX *store) { return 1; }

	using SSL_ptr_t = std::unique_ptr<SSL, std::function<void(SSL*)> >;
	using BIO_ptr_t = std::unique_ptr<BIO, std::function<void(BIO*)> >;
	using SSL_CTX_ptr_t = std::unique_ptr<SSL_CTX, std::function<void(SSL_CTX*)> >;


	SSL_ptr_t ssl_{nullptr, [this](SSL* ssl) { SSL_free(ssl); }};
	BIO_ptr_t     sbio_{nullptr, [this](BIO* bio) { }}; // SSL_free do the job
	SSL_CTX_ptr_t ssl_ctx_{nullptr, [this](SSL_CTX* ssl_ctx) { SSL_CTX_free(ssl_ctx); }};

	int           sock_{-1};
	std::string   log_prefix_;
	method_t        meth_;
};

} // namespace

