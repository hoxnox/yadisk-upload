#include <logging.hpp>

std::unique_ptr<SimpleLogStream> SimpleLogStream::instance_;
bool                             SimpleLogStream::verbose = false;
bool                             SimpleLogStream::align   = false;
std::ostream&                    vlog                     = std::cout;
std::ostream&                    ilog                     = std::cout;
std::ostream&                    elog                     = std::cerr;

