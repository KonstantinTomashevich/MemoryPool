#include "CommonCases.hpp"

#include <Memory/UnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (UnorderedPool)

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    bool destructedFlag = false;
    Memory::UnorderedPool pool {
        32u, sizeof (NonTrivialData),
        [] (void *chunk)
        {
            new (chunk) NonTrivialData ();
        },
        [&destructedFlag] (void *chunk)
        {
            static_cast <NonTrivialData *> (chunk)->~NonTrivialData ();
            destructedFlag = true;
        }};

    TestPoolAcquireFree (pool, NonTrivialData (), destructedFlag);
}

BOOST_AUTO_TEST_SUITE_END ()