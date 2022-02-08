#include <lhmiscutil/singleton.h>

namespace LHMiscUtilNS
{
    SingletonException::SingletonException( const std::string& failure )
    : std::runtime_error( failure )
    {
    }

    SingletonException::~SingletonException()
    {
    }
}
