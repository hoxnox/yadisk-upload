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

#include <errno.h>
#include <ifaddrs.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <Logging.hpp>
#include <yandex/tls_context.hpp>
#include "TLSEcho.hpp"

#ifndef THORW
class EchoException : public std::exception
{
public:
	EchoException(std::string msg = "") { ss_ << msg; }
	EchoException(const EchoException& copy) { operator=(copy); };
	EchoException& operator=(const EchoException& copy)
		{ ss_ << copy.ss_.str(); return *this; }

	const char* what() const noexcept(true) override
		{ ss_copy_ = ss_.str(); return ss_copy_.c_str(); }

	EchoException& operator<(std::ostream& ss) { return *this; }
	std::stringstream& _stream() { return ss_; }
private:
	std::stringstream ss_;
	mutable std::string ss_copy_;
};
#define THROW for(EchoException e; true; ) throw e < e._stream()
#endif

void
TLSEcho::loop(std::string saddr, unsigned short port)
{
	int sock;
	int on = 1;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(saddr.c_str());
	addr.sin_port = htons(port);
	sock = socket(addr.sin_family, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1 || setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0)
	{
		THROW << _(" TLSEcho: error initializing socket.")
		      << _(" Message: ") << strerror(errno);
	}
	char tmp[50];
	memset(tmp, 0, sizeof(tmp));
	if (inet_ntop(AF_INET, &addr.sin_addr, tmp, sizeof(tmp)) == NULL)
	{
		THROW << _(" TLSEcho: given wrong address.")
		      << _(" Message: ") << strerror(errno);
	}
	VLOG << this << (" TLSEcho: binding to address ") << tmp << ":" << port;
	if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		THROW << _(" TLSEcho: error binding socket")
		      << _(" Message: ") << strerror(errno);
	}
	if (listen(sock, 1) == -1)
	{
		THROW << _(" TLSEcho: error calling listen.")
		      << _(" Message: ") << strerror(errno);
	}
	VLOG << this << (" TLSEcho: socket initialized. Starting event loop.");

	const int MAX_EPOLL_EVENTS = 2;
	int epoll = epoll_create(1);
	if (epoll == -1)
	{
		THROW << _(" TLSEcho: error epoll creating.")
		      << _(" Message: ") << strerror(errno);
	}

	struct epoll_event ev;
	ev.events = EPOLLERR | EPOLLRDHUP | EPOLLIN  | EPOLLPRI;
	const uint32_t NOT_CONNECTED = 0;
	ev.data.u32 = NOT_CONNECTED;
	if (epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &ev) == -1)
	{
		THROW << _(" TLSEcho: error registering epoll events.")
		      << _(" Message: ") << strerror(errno);
	}

	stop_ = false;
	while (!stop_)
	{
		struct epoll_event triggered[MAX_EPOLL_EVENTS];
		int n = epoll_wait(epoll, triggered, MAX_EPOLL_EVENTS, 1000);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			THROW << _(" TLSEcho: error epoll wait.")
			      << _(" Message: ") << strerror(errno);
		}
		for (int i = 0; i < n; ++i)
		{
			if (triggered[i].events == EPOLLERR)
			{
				THROW << _("TLSEcho: socket error.")
				      << _(" Message: ") << strerror(errno);
			}
			else if (triggered[i].events == EPOLLRDHUP)
			{
				VLOG << _("Dosconnected.")
				     << _(" Message: ") << strerror(errno);
				shutdown(sock, SHUT_RDWR);
			}
			else if ((triggered[i].events & EPOLLIN) ||
			         (triggered[i].events & EPOLLPRI))
			{
				struct sockaddr_in raddr;
				VLOG << this << _(" TLSEcho: trying accept.");
				socklen_t raddr_len = sizeof(raddr);
				std::stringstream log_prefix;
				int sock2 =accept(sock, (sockaddr*)&raddr, &raddr_len);
				if (sock2 == -1)
				{
					THROW << _(" TLSEcho: error calling accept")
					      << _(" Message: ") << strerror(errno);
				}
				log_prefix << this;
				yandex::tls_context ctx(sock2, yandex::tls_context::method_t::SRV,
						log_prefix.str(), cert_, pkey_);
				int tlsrs = ctx.start();
				if (tlsrs != 0)
				{
					THROW << _(" TLSEcho: error calling TLS context Start.")
					      << _(" Message: ") << strerror(errno);
				}
				char tmp[50];
				memset(tmp, 0, sizeof(tmp));
				if (inet_ntop(AF_INET, &raddr.sin_addr, tmp, sizeof(tmp)))
					VLOG << this << _(" TLSEcho: accepted from ") << tmp
					     << ":" << ntohs(raddr.sin_port) << ". Mirroring.";
				else
					VLOG << this << _(" TLSEcho: accepted from <error translating>")
					     << ". Message: " << strerror(errno) << ". Mirroring.";
				char buf[0x10000];
				memset(buf, 0, sizeof(buf));
				do
				{
					int rs = SSL_read(ctx.ssl(), buf, sizeof(buf));
					if (rs == -1)
					{
						ELOG << _(" TLSEcho: error data receiving.")
						     << _(" rs = ") << rs
						     << _(" Message: ") << strerror(errno)
						     << _(" SSL code: ") << ERR_peek_last_error()
						     << _(" SSL Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
						break;
					}
					if (rs == 0)
						break;
					for (size_t i = 0; rs > 0 && i < multiplier_; ++i)
					{
						int rs2 = SSL_write(ctx.ssl(), buf, rs);
						if (rs2 != rs)
						{
							ELOG << _(" TLSEcho: error data sending.")
							     << _(" rs = ") << rs
							     << _(" Message: ") << strerror(errno)
							     << _(" SSL code: ") << ERR_peek_last_error()
							     << _(" SSL Message: ") << ERR_error_string(ERR_peek_last_error(), nullptr);
							break;
						}
					}
				} while(!oneshot_);
				//std::cout << "DATA: " << buf << std::endl;
				SSL_shutdown(ctx.ssl());
				shutdown(sock2, SHUT_RDWR);
				close(sock2);
			}
		}
	}
}

const std::vector<uint8_t> TLSEcho::DEFAULT_CERT = {
};

const std::vector<uint8_t> TLSEcho::DEFAULT_KEY = {
};

