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

template <typename Pool, typename Value>
void TestPoolAcquireFree (Pool &pool, const Value &defaultValue, bool &destructedFlag)
{
    auto *chunk = static_cast <Value *> (pool.Acquire ());
    BOOST_REQUIRE (chunk);
    BOOST_REQUIRE (*chunk == defaultValue);
    pool.Free (chunk);
    BOOST_REQUIRE (destructedFlag);
}
