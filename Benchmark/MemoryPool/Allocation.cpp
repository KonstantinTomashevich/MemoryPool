#include <cstdlib>
#include <functional>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include "Types.hpp"

template <typename ObjectType>
static void AllocateOnly_NewDelete (benchmark::State &state)
{
    std::vector <ObjectType *> allocated;
    for (auto _ : state)
    {
        auto *object = new ObjectType ();
        state.PauseTiming ();
        allocated.emplace_back (object);
        state.ResumeTiming ();
    }

    for (ObjectType *object : allocated)
    {
        delete object;
    }
}

template <typename ObjectType>
static void AllocateOnly_BoostObjectPool (benchmark::State &state)
{
    boost::object_pool <ObjectType> pool;
    for (auto _ : state)
    {
        auto *object = pool.construct ();
        benchmark::DoNotOptimize (object);
    }

    // Pool with destruct everything.
}

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component1032b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component1032b);