#include "CommonCases.hpp"

#include <Memory/TypedUnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (TypedUnorderedPool)

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
        NonTrivialData, Memory::EntryDefaultConstructor, CustomNonTrivialDataDestructor> pool {32u};
    TestPoolAcquireFree (pool, NonTrivialData (), nonTrivialDataDestructorCalled);
}

BOOST_AUTO_TEST_SUITE_END ()