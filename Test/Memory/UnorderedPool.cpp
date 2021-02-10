#include "CommonCases.hpp"

#include <Memory/UnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (UnorderedPool)

#define DEFAULT_PAGE_CAPACITY 32u

void NonTrivialDataConstructor (void *chunk) noexcept
{
    new (chunk) NonTrivialData ();
}

void NonTrivialDataDestructor (void *chunk) noexcept
{
    static_cast <NonTrivialData *> (chunk)->~NonTrivialData ();
}

static bool nonTrivialDataDestructorCalled = false;

void CustomNonTrivialDataDestructor (void *chunk) noexcept
{
    nonTrivialDataDestructorCalled = true;
    NonTrivialDataDestructor (chunk);
}

static Memory::UnorderedPool ConstructDefaultPool ()
{
    return Memory::UnorderedPool (
        DEFAULT_PAGE_CAPACITY, sizeof (NonTrivialData),
        NonTrivialDataConstructor, NonTrivialDataDestructor);
}

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    nonTrivialDataDestructorCalled = false;
    Memory::UnorderedPool pool {
        DEFAULT_PAGE_CAPACITY, sizeof (NonTrivialData),
        NonTrivialDataConstructor, CustomNonTrivialDataDestructor};

    TestPoolAcquireFree (pool, NonTrivialData (), nonTrivialDataDestructorCalled);
}

BOOST_AUTO_TEST_CASE (AcquirePageCount)
{
    Memory::UnorderedPool pool = ConstructDefaultPool ();
    TestAnyPoolAcquirePageCount (pool);
}

BOOST_AUTO_TEST_CASE (Shrink)
{
    Memory::UnorderedPool pool = ConstructDefaultPool ();
    TestAnyPoolShrink (pool);
}

BOOST_AUTO_TEST_SUITE_END ()