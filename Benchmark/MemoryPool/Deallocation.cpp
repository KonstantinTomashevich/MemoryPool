#include <cstdlib>
#include <functional>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include "Types.hpp"

#define POOL_MAX_SIZE 10000u

template <typename ObjectType, typename AllocatorLambda, typename DeallocatorLambda>
void DeallocationRoutine (benchmark::State &state, const AllocatorLambda &allocator,
                          const DeallocatorLambda &deallocator)
{
    std::vector <ObjectType *> allocated;
    for (auto _ : state)
    {
        state.PauseTiming ();
        if (allocated.size() < POOL_MAX_SIZE / 10u)
        {
            for (std::size_t index = 0; index < POOL_MAX_SIZE; ++index)
            {
                allocated.emplace_back (allocator ());
            }
        }

        state.ResumeTiming ();
        deallocator (allocated.back ());
        allocated.pop_back ();
    }

    for (ObjectType *left : allocated)
    {
        deallocator (left);
    }
}

template <typename ObjectType>
static void DeallocateOnly_NewDelete (benchmark::State &state)
{
    DeallocationRoutine <ObjectType> (
        state,
        [] ()
        {
            return new ObjectType ();
        },
        [] (ObjectType *object)
        {
            delete object;
        });
}

template <typename ObjectType>
static void DeallocateOnly_BoostObjectPool (benchmark::State &state)
{
    boost::object_pool <ObjectType> pool;
    DeallocationRoutine <ObjectType> (
        state,
        [&pool] ()
        {
            return pool.construct ();
        },
        [&pool] (ObjectType *object)
        {
            pool.destroy (object);
        });
}

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component_32b);

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component_192b);

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component_1032b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component_32b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component_192b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component_1032b);