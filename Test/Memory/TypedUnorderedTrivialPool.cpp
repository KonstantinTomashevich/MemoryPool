#include "CommonCases.hpp"

#include <Memory/TypedUnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (TypedUnorderedTrivialPool)

#define DEFAULT_PAGE_CAPACITY 32u

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    Memory::TypedUnorderedTrivialPool <TrivialData> pool {DEFAULT_PAGE_CAPACITY};
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
    Memory::TypedUnorderedTrivialPool <TrivialData> pool {DEFAULT_PAGE_CAPACITY};
    TestAnyPoolAcquirePageCount (pool);
}

BOOST_AUTO_TEST_CASE (Shrink)
{
    Memory::TypedUnorderedTrivialPool <TrivialData> pool {DEFAULT_PAGE_CAPACITY};
    TestAnyPoolShrink(pool);
}

BOOST_AUTO_TEST_SUITE_END ()