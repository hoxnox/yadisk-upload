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

#include <yandex/transport.hpp>
#include <logging.hpp>

std::ostream&
operator<<(std::ostream& os, const yandex::transport::result_t& rs)
{
	if (rs.type() == yandex::transport::result_t::SUCCESS)
	{
		os << _("Transport operation was completed successful.");
		return os;
	}
	else if (rs.type() == yandex::transport::result_t::INPROGRESS)
	{
		os << _("Transport operation is in progress.");
		return os;
	}
	else if (rs.type() == yandex::transport::result_t::CANCELED)
	{
		os << _("Transport operation was canceled.");
	}
	else
	{
		os << _("Transport operation was failed.");
	}
	
	if (rs.code() != 0)
		os << _(" Code: ") << rs.code();
	if (!rs.message().empty())
		os << _(" Message: ") << rs.message();

	return os;
}

