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
            static OneTimeCreate< T > oneTimeCreated( std::forward<Args>( args )... );
            return oneTimeCreated.Get()
        }

    private:
        template< class... Args >
        OneTimeCreate( Args&&... args )
            : ptr( std::make_shared< T >( std::forward<Args>( args )... ) )
            , ptrMutex()
        {
            if ( !ptr )
            {
                std::ostringstream oss;

                oss << "OneTimeCreate of [" << OneTimeCreateTraits< T >::name << "] failed";

                throw std::runtime_error( oss.str() );
            }
        }

        std::shared_ptr< T > Get()
        {
            const std::lock_guard<std::mutex> lock( ptrMutex );
            if ( ptr )
            {
                return std::move( ptr );
            }
            else
            {
                return nullptr;
            }
        }

        std::shared_ptr< T > ptr;
        std::mutex ptrMutex;
    };
}

#endif