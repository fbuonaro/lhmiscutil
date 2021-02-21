#include <lhutil/singleton.h>

namespace LHUtilNS
{
    SingletonException::SingletonException( const std::string& failure )
    : std::runtime_error( failure )
    {
    }

    SingletonException::~SingletonException()
    {
    }
}
