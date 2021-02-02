#include "CommonCases.hpp"

#include <Memory/UnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (UnorderedPool)

#define DEFAULT_PAGE_CAPACITY 32u

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    bool destructedFlag = false;
    Memory::UnorderedPool pool {
        DEFAULT_PAGE_CAPACITY, sizeof (NonTrivialData),
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

BOOST_AUTO_TEST_CASE (AcquirePageCount)
{
    Memory::UnorderedPool pool {
        DEFAULT_PAGE_CAPACITY, sizeof (NonTrivialData),
        [] (void *chunk)
        {
            new (chunk) NonTrivialData ();
        },
        [] (void *chunk)
        {
            static_cast <NonTrivialData *> (chunk)->~NonTrivialData ();
        }};

    TestAnyPoolAcquirePageCount (pool);
}

BOOST_AUTO_TEST_SUITE_END ()