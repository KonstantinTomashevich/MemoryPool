#pragma once

#include <cstdint>
#include <vector>

#include <boost/test/unit_test.hpp>

struct TrivialData
{
    uint8_t a_;
    uint8_t b_;
    uint8_t c_;
    uint8_t d_;
    uint64_t otherValue_;
};

static_assert (std::is_trivial_v <TrivialData>);

struct NonTrivialData
{
    std::vector <uint32_t> values_ {};
    uint32_t first_ = 1u;
    uint32_t second_ = 2u;

    bool operator == (const NonTrivialData &other) const;

    bool operator != (const NonTrivialData &other) const;
};

static_assert (!std::is_trivial_v <NonTrivialData>);

template <typename Pool, typename ChunkEditor>
void TestTrivialPoolAcquireFree (Pool &pool, const ChunkEditor &chunkEditor)
{
    auto *chunk = pool.Acquire ();
    BOOST_REQUIRE (chunk);

    // Edit chunk data. If it's not correctly allocated block, test could crash.
    chunkEditor (chunk);
    pool.Free (chunk);
}

template <typename Pool, typename RealValueType>
void TestPoolAcquireFree (Pool &pool, const RealValueType &defaultValue, bool &destructedFlag)
{
    auto *chunk = static_cast <RealValueType *> (pool.Acquire ());
    BOOST_REQUIRE (chunk);
    BOOST_REQUIRE (*chunk == defaultValue);
    pool.Free (chunk);
    BOOST_REQUIRE (destructedFlag);
}

template <typename Pool>
void TestAnyPoolAcquirePageCount (Pool &pool)
{
    BOOST_REQUIRE_MESSAGE (pool.GetPageCount () == 0u, "New empty pool must have 0 pages.");
    for (uint32_t itemIndex = 0u; itemIndex < pool.GetPageCapacity () * 2u + 1u; ++itemIndex)
    {
        pool.Acquire ();
        BOOST_REQUIRE(pool.GetPageCount () == 1u + itemIndex / pool.GetPageCapacity ());
    }
}

template <typename Pool>
void TestAnyPoolShrink (Pool &pool)
{
    BOOST_REQUIRE_MESSAGE (pool.GetPageCount () == 0u, "New empty pool must have 0 pages.");
    std::vector <std::vector <typename Pool::ValueType *>> valuesPerPage;
    valuesPerPage.resize (2u, {});

    for (uint32_t itemIndex = 0u; itemIndex < pool.GetPageCapacity () * 2u; ++itemIndex)
    {
        valuesPerPage[itemIndex / pool.GetPageCapacity ()].push_back (pool.Acquire ());
    }

    BOOST_REQUIRE (pool.GetPageCount () == 2u);
    pool.Shrink ();
    BOOST_REQUIRE (pool.GetPageCount () == 2u);

    // Free half of first and half of second pages values.
    for (auto &pageValues : valuesPerPage)
    {
        for (uint32_t itemIndex = pageValues.size () / 2u; itemIndex < pageValues.size (); ++itemIndex)
        {
            pool.Free (pageValues[itemIndex]);
        }

        pageValues.resize (pageValues.size () / 2u);
    }

    pool.Shrink ();
    BOOST_REQUIRE (pool.GetPageCount () == 2u);

    auto clearPage = [&valuesPerPage, &pool] (uint32_t pageIndex)
    {
        for (uint32_t itemIndex = 0u; itemIndex < valuesPerPage[pageIndex].size (); ++itemIndex)
        {
            pool.Free (valuesPerPage[pageIndex][itemIndex]);
        }

        valuesPerPage[pageIndex].clear ();
    };

    clearPage (0u);
    pool.Shrink ();
    // TODO: Check that used chunks were not destructed (for non-trivial pools)?
    BOOST_REQUIRE (pool.GetPageCount () == 1u);

    clearPage (1u);
    pool.Shrink ();
    BOOST_REQUIRE (pool.GetPageCount () == 0u);
}

// TODO: Test clean methods, check if destructors for non-trivial pools are called correctly.
