#include "CommonCases.hpp"

#include <Memory/UnorderedPool.hpp>

BOOST_AUTO_TEST_SUITE (UnorderedTrivialPool)

BOOST_AUTO_TEST_CASE (AcquireAndFree)
{
    Memory::UnorderedTrivialPool pool {32u, sizeof (TrivialData)};
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

BOOST_AUTO_TEST_SUITE_END ()