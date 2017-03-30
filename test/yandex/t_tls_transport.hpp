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

#include <memory>
#include <sstream>

#include <boost/filesystem.hpp>

#include <yandex/tls_transport.hpp>
#include <TlsEcho.hpp>

class TestTlsTransport : public testing::Test
{
public:
	void SetUp()
	{
		//tmpdir_ = boost::filesystem::unique_path("TestTlsTransport-%%%%");
		//boost::filesystem::create_directories(tmpdir_);
		tls_echo_ = std::make_unique<TlsEcho>("127.0.0.241", 0x4444);
		tls_echo_->Dispatch();
		t_ = std::make_unique<yandex::tls_transport>(
			"token", "127.0.0.241", 0x4444, true);
	}
	void TearDown()
	{
		//boost::filesystem::remove_all(tmpdir_.c_str());
		tls_echo_.reset(nullptr);
		EXPECT_TRUE(errlog.str().empty()) << errlog.str();
	}
	std::unique_ptr<TlsEcho> tls_echo_{nullptr};
	std::unique_ptr<yandex::tls_transport> t_{nullptr};
	boost::filesystem::path tmpdir_;
};

TEST_F(TestTlsTransport, get)
{
	size_t callback_called_count = 0;
	std::vector<uint8_t> buf;
	t_->get("api/get",
		[&callback_called_count, &buf](const std::string& url,
		                               const uint8_t* data,
		                               size_t datasz)
		{
			std::copy(data, data + datasz, std::back_inserter(buf));
			++callback_called_count;
		});
	EXPECT_EQ("Error parsing status line. Status: GET api/get HTTP/1.1\n", errlog.str());
	errlog.str("");
	EXPECT_EQ("GET api/get HTTP/1.1\r\n"
	          "Host: 127.0.0.241\r\n"
	          "User-Agent: hoxnox/yadisk-upload\r\n"
	          "Accept: */*\r\n"
	          "Authorization: OAuth token\r\n\r\n"
	          , std::string(buf.begin(), buf.end()));
	EXPECT_LT(0, callback_called_count);
}

TEST_F(TestTlsTransport, put)
{
	std::stringstream ss;
	ss.str("DATA");
	std::vector<uint8_t> buf;
	size_t callback_called_count = 0;
	t_->put("api/put", ss, 0,
		[&callback_called_count, &buf](const std::string& url,
		                               const uint8_t* data,
		                               size_t datasz)
		{
			std::copy(data, data + datasz, std::back_inserter(buf));
			++callback_called_count;
		});
	EXPECT_EQ("Error parsing status line. Status: PUT api/put HTTP/1.1\n", errlog.str());
	errlog.str("");
	EXPECT_EQ("PUT api/put HTTP/1.1\r\n"
	          "Host: 127.0.0.241\r\n"
	          "User-Agent: hoxnox/yadisk-upload\r\n"
	          "Accept: */*\r\n"
	          "Content-Length: 4\r\n"
	          "Content-Type: application/octet-stream\r\n\r\n"
	          "DATA"
	          , std::string(buf.begin(), buf.end()));
	EXPECT_LT(0, callback_called_count);
}

