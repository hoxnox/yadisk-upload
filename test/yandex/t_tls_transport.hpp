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

#include <yandex/tls_transport.hpp>
#include <TLSEcho.hpp>

class TestTlsTransport : public testing::Test
{
public:
	void SetUp()
	{
		tls_server.Dispatch("127.0.0.241", 0x5555);
	}
	void TearDown()
	{
		tls_server.Stop();
	}
protected:
	TLSEcho tls_server;
};

TEST(TestTlsTransport, get)
{
	yandex::tls_transport t("get");
	t.get("yandex.ru/api", [](const std::string& data)
		{
			EXPECT_EQ("", data);
		});
}

TEST(TestTlsTransport, put)
{
	yandex::tls_transport t("put");
	t.put("yandex.ru/api", [](const std::string& data)
		{
			EXPECT_EQ("", data);
		});
}

