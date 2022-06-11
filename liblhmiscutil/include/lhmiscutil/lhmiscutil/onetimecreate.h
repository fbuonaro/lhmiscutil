#ifndef __LHMISCUTIL_ONETIMECREATE_H__
#define __LHMISCUTIL_ONETIMECREATE_H__

#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace LHMiscUtilNS
{
    template< typename T >
    class OneTimeCreateTraits
    {
    public:
        static constexpr const bool enabled = false;
        static constexpr const char* name = "_UNKNOWN_";
    };

#define EnableClassAsOneTimeCreate( classPath ) \
    template<> \
    class OneTimeCreateTraits< classPath > \
    { \
        public: \
           static constexpr const bool enabled = true; \
           static constexpr const char* name = #classPath; \
    }

    template< typename T >
    class OneTimeCreate
    {
        static_assert( OneTimeCreateTraits< T >::enabled,
            "Class is not enabled for OneTimeCreate, use EnableClassAsOneTimeCreate" );

    public:
        template< class... Args >
        static std::shared_ptr< T > Create( Args&&... args )
        {
            return oneTimeCreate.create( std::forward<Args>( args )... );
        }

    private:
        OneTimeCreate()
            : hasBeenCreated( false )
            , onceMutex()
        {
        }

        template< class... Args >
        std::shared_ptr< T > create( Args&&... args )
        {
            const std::lock_guard<std::mutex> lock( onceMutex );
            if ( !hasBeenCreated )
            {
                auto once = std::make_shared< T >( std::forward<Args>( args )... );

                if ( once )
                {
                    hasBeenCreated = true;
                }

                return once;
            }
            else
            {
                return nullptr;
            }
        }

        bool hasBeenCreated;
        std::mutex onceMutex;
        static OneTimeCreate< T > oneTimeCreate;
    };

    template< typename T >
    OneTimeCreate< T > OneTimeCreate< T >::oneTimeCreate;
}

#endif