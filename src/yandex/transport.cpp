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
	if (rs.message().empty())
		os << _(" Message: ") << rs.message();

	return os;
}

