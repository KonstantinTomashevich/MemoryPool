#include "CommonCases.hpp"

#include <Memory/TypedUnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (TypedUnorderedPool)

#define DEFAULT_PAGE_CAPACITY 32u

static bool nonTrivialDataDestructorCalled = false;

void CustomNonTrivialDataDestructor (NonTrivialData *data) noexcept
{
    nonTrivialDataDestructorCalled = true;
    Memory::EntryDefaultDestructor (data);
}

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    nonTrivialDataDestructorCalled = false;
    Memory::TypedUnorderedPool <
        NonTrivialData, Memory::EntryDefaultConstructor, CustomNonTrivialDataDestructor> pool {DEFAULT_PAGE_CAPACITY};
    TestPoolAcquireFree (pool, NonTrivialData (), nonTrivialDataDestructorCalled);
}

BOOST_AUTO_TEST_CASE (AcquirePageCount)
{
    Memory::TypedUnorderedPool <NonTrivialData> pool {DEFAULT_PAGE_CAPACITY};
    TestAnyPoolAcquirePageCount (pool);
}

BOOST_AUTO_TEST_CASE (Shrink)
{
    Memory::TypedUnorderedPool <NonTrivialData> pool {DEFAULT_PAGE_CAPACITY};
    TestAnyPoolShrink (pool);
}

BOOST_AUTO_TEST_SUITE_END ()