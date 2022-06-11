#ifndef __LHMISCUTIL_SINGLETON_H__
#define __LHMISCUTIL_SINGLETON_H__

#include <memory>
#include <sstream>
#include <stdexcept>

namespace LHMiscUtilNS
{
    class SingletonException : public std::runtime_error
    {
    public:
        SingletonException( const std::string& failure );
        virtual ~SingletonException();
    };

    enum class SingletonCanBeSet
    {
        Once,
        WhenEmpty,
        Repeatedly
    };

    template< typename T >
    class SingletonTraits
    {
    public:
        static constexpr const bool enabled = false;
        static constexpr const char* name = "_UNKNOWN_";
        static constexpr const SingletonCanBeSet canBeSet = SingletonCanBeSet::Repeatedly;
    };

#define EnableClassAsSingleton( classPath, canBeSetdWhen ) \
    template<> \
    class SingletonTraits< classPath > \
    { \
        public: \
           static constexpr const bool enabled = true; \
           static constexpr const char* name = #classPath; \
           static constexpr const SingletonCanBeSet canBeSet = canBeSetdWhen; \
    }

    template< typename T >
    class Singleton
    {
        static_assert( SingletonTraits< T >::enabled,
            "Class is not enabled as singleton, use EnableClassAsSingleton" );
    public:
        static std::shared_ptr< T > GetInstance()
        {
            auto instance = singleton.getInstance();

            if ( !instance )
            {
                std::ostringstream oss;

                oss << "singleton for [" << SingletonTraits< T >::name << "] is empty";

                throw SingletonException( oss.str() );
            }

            return instance;
        }

        static void SetInstance( const std::shared_ptr< T >& instance )
        {
            if ( !instance )
            {
                std::ostringstream oss;

                oss << "singleton for [" << SingletonTraits< T >::name << "] passed null instance";

                throw SingletonException( oss.str() );
            }

            switch ( SingletonTraits< T >::canBeSet )
            {
            case( SingletonCanBeSet::Once ):
            {
                if ( singleton.hasBeenSet )
                {
                    std::ostringstream oss;

                    oss << "singleton for [" << SingletonTraits< T >::name << "] can only be set once";

                    throw SingletonException( oss.str() );
                }

                break;
            }
            case( SingletonCanBeSet::WhenEmpty ):
            {
                if ( auto existingInstance = singleton.getInstance() )
                {
                    std::ostringstream oss;

                    oss << "singleton for [" << SingletonTraits< T >::name << "] can only be set when empty";

                    throw SingletonException( oss.str() );
                }

                break;
            }
            default:
            case( SingletonCanBeSet::Repeatedly ):
            {
                break;
            }
            }

            singleton.setInstance( instance );
        }

    private:
        // Private implementation of a container with a handle to a T
        Singleton()
            : instanceHandle()
            , hasBeenSet( false )
        {
        }

        std::shared_ptr< T > getInstance()
        {
            return instanceHandle.lock();
        }

        void setInstance( const std::shared_ptr< T >& instance )
        {
            instanceHandle = instance;
            hasBeenSet = true;
        }

        std::weak_ptr< T > instanceHandle;
        bool hasBeenSet;

        // The static container of a T
        static Singleton< T > singleton;
    };

    template< typename T >
    Singleton< T > Singleton< T >::singleton = Singleton< T >();
}

#endif
