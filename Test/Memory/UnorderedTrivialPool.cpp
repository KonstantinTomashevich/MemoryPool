#include "CommonCases.hpp"

#include <Memory/UnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (UnorderedTrivialPool)

#define DEFAULT_PAGE_CAPACITY 32u

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    Memory::UnorderedTrivialPool pool {DEFAULT_PAGE_CAPACITY, sizeof (TrivialData)};
    TestTrivialPoolAcquireFree (
        pool,
        [] (void *chunk)
        {
            auto *data = static_cast <TrivialData *> (chunk);
            data->a_ = 12u;
            data->b_ = 255u;
            data->c_ = 99u;
            data->d_ = 125u;
            data->otherValue_ = 78912u;
        });
}

BOOST_AUTO_TEST_CASE (AcquirePageCount)
{
    Memory::UnorderedTrivialPool pool {DEFAULT_PAGE_CAPACITY, sizeof (TrivialData)};
    TestAnyPoolAcquirePageCount (pool);
}

BOOST_AUTO_TEST_SUITE_END ()