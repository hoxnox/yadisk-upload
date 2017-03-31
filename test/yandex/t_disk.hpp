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

#include <yandex/disk.hpp>
#include "transport_mock.hpp"

#include <boost/filesystem.hpp>

class TestYandexDisk : public ::testing::Test
{
protected:
	void SetUp()
	{
		errlog.str("");
		tmpdir_ = boost::filesystem::unique_path("TestTlsTransport-%%%%");
		boost::filesystem::create_directories(tmpdir_);
		transport = std::make_shared<transport_mock>();
		transport_mock::cmd.clear();
		api = std::make_unique<yandex::disk::api>(transport);
	}
	void TearDown()
	{
		boost::filesystem::remove_all(tmpdir_.c_str());
		api.reset(nullptr);
		transport = nullptr;
		EXPECT_TRUE(errlog.str().empty()) << errlog.str();
	}
	std::shared_ptr<yandex::transport> transport{nullptr};
	std::unique_ptr<yandex::disk::api> api{nullptr};
	boost::filesystem::path tmpdir_;
};

TEST_F(TestYandexDisk, upload_nonexist_source)
{
	EXPECT_FALSE(api->upload((tmpdir_/"nonexist").string(), "destination"));
	transport_mock* t = dynamic_cast<transport_mock*>(transport.get());
	EXPECT_TRUE(errlog.str().find("Error opening file. Filename: ") != std::string::npos);
	errlog.str("");
	EXPECT_EQ(0, transport_mock::cmd.size());
}

inline bool
write_file(boost::filesystem::path fname, std::vector<uint8_t> data)
{
	std::ofstream ofile(fname.string().c_str(), std::ios::binary);
	if (!ofile.is_open())
		return false;
	std::ostreambuf_iterator<char> writer(ofile);
	std::copy(data.begin(), data.end(), writer);
	return true;
}

TEST_F(TestYandexDisk, upload_bad_destination)
{
	write_file(tmpdir_/"source", {0x01, 0x02, 0x03, 0x04});
	EXPECT_FALSE(api->upload((tmpdir_/"source").string(), "bad"));
	transport_mock* t = dynamic_cast<transport_mock*>(transport.get());
	EXPECT_EQ("Error getting upload URL. Transport operation was failed. Code: 501 Message: Bad path.\n", errlog.str());
	errlog.str("");
	EXPECT_NE(0, transport_mock::cmd.size());
}

TEST_F(TestYandexDisk, upload_happy_path)
{
	write_file(tmpdir_/"source", {0x01, 0x02, 0x03, 0x04});
	EXPECT_TRUE(api->upload((tmpdir_/"source").string(), "destination дир"));
	transport_mock* t = dynamic_cast<transport_mock*>(transport.get());
	EXPECT_NE(0, transport_mock::cmd.size());
	std::vector<std::pair<transport_mock::methods, std::string>> etalon{
		{transport_mock::methods::GET, "/v1/disk/resources/upload?path=destination+%D0%B4%D0%B8%D1%80"},
		{transport_mock::methods::PUT, "/upload-target/20170323T090039.180.utd.cvczzjdwcbn59g8sqchqxfh32-k17h.1885364"}};
	EXPECT_EQ(etalon, transport_mock::cmd);
}

