#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include <lhmiscutil/singleton.h>

namespace LHMiscUtilTestNS
{
    using namespace LHMiscUtilNS;

    class BaseC
    {
        public:
            BaseC()
            {
            }

            virtual ~BaseC()
            {
            }

            virtual std::string GetIdentifier() const = 0;
    };

    namespace TestNS1
    {
        class BaseA : public BaseC
        {
            public:
                BaseA( int _number )
                :   BaseC()
                ,   number( _number )
                {
                }

                ~BaseA()
                {
                }

                virtual std::string GetIdentifier() const
                {
                    std::ostringstream oss;
                    oss << GetNumber();
                    return oss.str();
                }

                int GetNumber() const
                {
                    return number;
                }

            private:
                int number;
        };
    }
    
    namespace TestNS2
    {
        class BaseB : public BaseC
        {
            public:
                BaseB( const std::string& _name )
                :   BaseC()
                ,   name( _name )
                {
                }

                virtual ~BaseB()
                {
                }

                std::string GetIdentifier() const
                {
                    return GetName();
                }

                virtual std::string GetName() const
                {
                    return name;
                }

            private:
                std::string name;
        };
    }

}

namespace LHMiscUtilNS
{
    using namespace LHMiscUtilTestNS;
    using namespace LHMiscUtilTestNS::TestNS1;

    EnableClassAsSingleton( BaseC, SingletonCanBeSet::Once );
    EnableClassAsSingleton( BaseA, SingletonCanBeSet::Repeatedly );
    EnableClassAsSingleton( TestNS2::BaseB, SingletonCanBeSet::WhenEmpty );
}

namespace LHMiscUtilTestNS
{
    using namespace TestNS1;
    using namespace TestNS2;

    class ClassA1 : public BaseA
    {
        public:
            ClassA1( int number )
            :   BaseA( number )
            {
            }
    };

    class ClassA2 : public BaseA
    {
        public:
            ClassA2()
            :   BaseA( -1 )
            {
            }

            std::string GetIdentifier() const
            {
                return "ClassA2";
            }
    };

    class ClassB1 : public BaseB
    {
        public:
            ClassB1( const std::string& name )
            :   BaseB( name )
            {
            }
    };

    class ClassB2 : public BaseB
    {
        public:
            ClassB2()
            :   BaseB( "" )
            {
            }

            std::string GetName() const
            {
                return "ClassB2";
            }
    };


    class SingletonTest : public ::testing::Test
    {
            virtual void SetUp()
            {   
            }

            virtual void TearDown()
            {
            }
    };


    TEST_F( SingletonTest, TestGetEmptyThrows1 )
    {
        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            auto instance = Singleton< BaseC >::GetInstance();
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [BaseC] is empty", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestGetEmptyThrows2 )
    {
        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            auto instance = Singleton< TestNS1::BaseA >::GetInstance();
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [BaseA] is empty", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestGetEmptyThrows3 )
    {
        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            auto instance = Singleton< BaseB >::GetInstance();
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [TestNS2::BaseB] is empty", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestSetNullThrows1 )
    {
        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            Singleton< BaseC >::SetInstance( nullptr );
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [BaseC] passed null instance", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestSetNullThrows2 )
    {
        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            Singleton< BaseC >::SetInstance( std::shared_ptr< LHMiscUtilTestNS::BaseC >() );
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [BaseC] passed null instance", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestSetGetOnceSuccess )
    {
        auto a = std::shared_ptr< LHMiscUtilTestNS::TestNS1::BaseA >( new ClassA2() );
        auto b = std::shared_ptr< LHMiscUtilTestNS::TestNS2::BaseB >( new ClassB2() );
        Singleton< TestNS1::BaseA >::SetInstance( a );
        Singleton< BaseB >::SetInstance( b );

        std::shared_ptr< BaseC > instanceA = Singleton< TestNS1::BaseA >::GetInstance();
        std::shared_ptr< TestNS2::BaseB > instanceB = Singleton< TestNS2::BaseB >::GetInstance();

        ASSERT_TRUE( instanceA != nullptr );
        EXPECT_EQ( std::string( "ClassA2" ), instanceA->GetIdentifier() );

        ASSERT_TRUE( instanceB != nullptr );
        EXPECT_EQ( std::string( "ClassB2" ), instanceB->GetIdentifier() );
    }

    TEST_F( SingletonTest, TestSetGetOutOfScopeThrows )
    {
        {
            auto c = std::shared_ptr< LHMiscUtilTestNS::BaseC >( new ClassA1( 1 ) );
            Singleton< BaseC >::SetInstance( c );
            std::shared_ptr< BaseC > instanceAa = Singleton< BaseC >::GetInstance();
            ASSERT_TRUE( instanceAa != nullptr );
            EXPECT_EQ( std::string( "1" ), instanceAa->GetIdentifier() );
        }

        bool exceptionThrown = false;
        std::string msg = "";

        try
        {
            auto instance = Singleton< BaseC >::GetInstance();
        }
        catch( const SingletonException& se )
        {
            msg = se.what();
            exceptionThrown = true;
        }
        catch( ... )
        {
            exceptionThrown = true;
        }

        EXPECT_EQ( "singleton for [BaseC] is empty", msg );
        EXPECT_TRUE( exceptionThrown );
    }

    TEST_F( SingletonTest, TestSetGetWhenEmptySuccessOutOfScopeSetSuccess )
    {
        auto b1 = std::shared_ptr< LHMiscUtilTestNS::TestNS2::BaseB >( new ClassB1( "testB1" ) );

        {
            auto b2 = std::shared_ptr< LHMiscUtilTestNS::TestNS2::BaseB >( new ClassB1( "testB2" ) );
            Singleton< BaseB >::SetInstance( b2 );
            std::shared_ptr< TestNS2::BaseB > instanceB = Singleton< TestNS2::BaseB >::GetInstance();
            ASSERT_TRUE( instanceB != nullptr );
            EXPECT_EQ( std::string( "testB2" ), instanceB->GetIdentifier() );

            bool exceptionThrown = false;
            std::string msg = "";

            try
            {
                Singleton< BaseB >::SetInstance( b1 );
            }
            catch( const SingletonException& se )
            {
                msg = se.what();
                exceptionThrown = true;
            }
            catch( ... )
            {
                exceptionThrown = true;
            }

            EXPECT_EQ( "singleton for [TestNS2::BaseB] can only be set when empty", msg );
            EXPECT_TRUE( exceptionThrown );
        }

        Singleton< BaseB >::SetInstance( b1 );
        std::shared_ptr< TestNS2::BaseB > instanceB = Singleton< TestNS2::BaseB >::GetInstance();
        ASSERT_TRUE( instanceB != nullptr );
        EXPECT_EQ( std::string( "testB1" ), instanceB->GetIdentifier() );
    }

    TEST_F( SingletonTest, TestSetGetRepeatedlySuccess )
    {
        auto a = std::shared_ptr< LHMiscUtilTestNS::TestNS1::BaseA >( new ClassA2() );
        Singleton< TestNS1::BaseA >::SetInstance( a );
        std::shared_ptr< BaseC > instanceA = Singleton< TestNS1::BaseA >::GetInstance();

        auto a2 = std::shared_ptr< LHMiscUtilTestNS::TestNS1::BaseA >( new ClassA1( 123 ) );
        Singleton< TestNS1::BaseA >::SetInstance( a2 );
        std::shared_ptr< BaseC > instanceA2 = Singleton< TestNS1::BaseA >::GetInstance();

        ASSERT_TRUE( instanceA != nullptr );
        EXPECT_EQ( std::string( "ClassA2" ), instanceA->GetIdentifier() );
        ASSERT_TRUE( instanceA2 != nullptr );
        EXPECT_EQ( std::string( "123" ), instanceA2->GetIdentifier() );
    }
}
