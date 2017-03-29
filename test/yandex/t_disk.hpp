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
		tmpdir_ = boost::filesystem::unique_path("TestTlsTransport-%%%%");
		boost::filesystem::create_directories(tmpdir_);
		transport = std::make_shared<transport_mock>();
		api = std::make_unique<yandex::disk::api>(transport);
	}
	void TearDown()
	{
		boost::filesystem::remove_all(tmpdir_.c_str());
		api.reset(nullptr);
		transport = nullptr;
	}
	std::shared_ptr<yandex::transport> transport{nullptr};
	std::unique_ptr<yandex::disk::api> api{nullptr};
	boost::filesystem::path tmpdir_;
};

TEST_F(TestYandexDisk, upload_nonexist_source)
{
	EXPECT_FALSE(api->upload((tmpdir_/"nonexist").string(), "destination"));
	transport_mock* t = dynamic_cast<transport_mock*>(transport.get());
	EXPECT_EQ(0, t->cmd_.size());
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

TEST_F(TestYandexDisk, upload_nonexist_destination)
{
	write_file(tmpdir_/"source", {0x01, 0x02, 0x03, 0x04});
	EXPECT_FALSE(api->upload((tmpdir_/"source").string(), "destination"));
	transport_mock* t = dynamic_cast<transport_mock*>(transport.get());
	EXPECT_NE(0, t->cmd_.size());
}

