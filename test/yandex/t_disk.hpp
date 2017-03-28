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

class TestYandexDisk : public ::testing::Test
{
protected:
	void SetUp()
	{
		transport = std::make_shared<transport_mock>();
	}
	std::shared_ptr<yandex::transport> transport{nullptr};
};

TEST_F(TestYandexDisk, ctor)
{
	yandex::disk::api api(transport);
}

