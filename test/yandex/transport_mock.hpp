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

#include <utility>
#include <vector>

#include <yandex/transport.hpp>

class transport_mock : public yandex::transport
{
public:
	transport_mock() : transport() {}
	~transport_mock() {}

	result_t
	get(std::string url, response_handler_t handler = nullptr) override
	{
		cmd.emplace_back(methods::GET, url);
		if (url == "/v1/disk/resources/upload?path=bad")
		{ // simulate bad path
			if (handler)
				handler(url, NULL, 0);
			return {transport::result_t::FAILED, 501, "Bad path."};
		}
		std::string ans = get_ans(url);
		if (ans.empty())
			return {transport::result_t::FAILED};
		if (handler)
			handler(url, (const uint8_t*)ans.data(), ans.length());
		return {transport::result_t::SUCCESS};
	}

	result_t
	put(std::string url,
	    std::basic_istream<char>& body,
	    size_t bodysz = 0,
	    response_handler_t handler = nullptr) override
	{
		cmd.emplace_back(methods::PUT, url);
		std::string ans = get_ans(url);
		if (ans.empty())
			return {transport::result_t::FAILED};
		if (handler)
			handler(url, (const uint8_t*)ans.data(), ans.length());
		return {transport::result_t::SUCCESS};
	}

	void cancel(uint16_t code = 0, std::string message = "")
	{
	}

	std::shared_ptr<transport>
		make_transport(std::string host, uint16_t port, size_t chunksz = 0) override
			{ return std::make_shared<transport_mock>(); }

	enum class methods : uint8_t
	{
		GET,
		PUT
	};

	static std::vector<std::pair<methods, std::string>> cmd;

	std::string
	get_ans(const std::string& url) const
	{
		std::string rs;
		size_t prefix_len = 0;
		for (const auto& ans : answers)
		{
			if (url.length() < ans.first.length() || prefix_len >= ans.first.length())
				continue;
			if (url.substr(0, ans.first.length()) == ans.first)
			{
				rs = ans.second;
				prefix_len = ans.first.length();
			}
		}
		return rs;
	}

private:
	std::string token_;
	const std::map<std::string, std::string> answers =
	{
		{
			"/v1/disk/resources/upload",
				"HTTP/1.1 200 OK\r\n"
				"Server: nginx\r\n"
				"Date: Thu, 23 Mar 2017 05:37:26 GMT\r\n"
				"Content-Type: application/json; charset=utf-8\r\n"
				"Content-Length: 160\r\n"
				"Connection: keep-alive\r\n"
				"Vary: Accept-Encoding\r\n"
				"Access-Control-Allow-Methods: PUT, POST, GET, OPTIONS\r\n"
				"Yandex-Cloud-Request-ID: rest-2d62e37e767a6fcc928a38204b0ef2d8-api05e\r\n"
				"Access-Control-Allow-Credentials: true\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"Access-Control-Allow-Headers: Accept-Language, Accept, X-HTTP-Method, X-Requested-With, Content-Type, Authorization\r\n"
				"\r\n"
				"{\"href\":\"https://uploader19m.disk.yandex.net:443/upload-target/20170323T090039.180.utd.cvczzjdwcbn59g8sqchqxfh32-k17h.1885364\",\"method\":\"PUT\",\"templated\":false}"
		},
		{
			"/upload-target/20170323T090039.180.utd.cvczzjdwcbn59g8sqchqxfh32-k17h.1885364",
				"HTTP/1.1 201 Created\r\n"
				"Server: nginx/1.8.1\r\n"
				"Date: Thu, 23 Mar 2017 06:01:11 GMT\r\n"
				"Transfer-Encoding: chunked\r\n"
				"Connection: keep-alive\r\n"
				"Keep-Alive: timeout=120\r\n"
				"Location: /fake/location\r\n"
				"\r\n"
		}
	};

};

std::vector<std::pair<transport_mock::methods, std::string>> transport_mock::cmd;

