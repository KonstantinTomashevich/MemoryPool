#include <cstdlib>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include "Types.hpp"

#define SAMPLE_SIZE 10000u

template <typename ObjectType, typename AllocatorLambda, typename DeallocatorLambda>
void AllocateDeallocateRoutine (benchmark::State &state, const AllocatorLambda &allocator,
                                const DeallocatorLambda &deallocator)
{
    std::array <ObjectType *, SAMPLE_SIZE> allocated {};
    std::size_t allocatedCount = 0u;

    for (auto _ : state)
    {
        // Start with filling full data sample.
        while (allocatedCount < SAMPLE_SIZE)
        {
            allocated[allocatedCount++] = allocator ();
        }

        // Drop half of objects to measure deallocation speed.
        while (allocatedCount > SAMPLE_SIZE / 2u)
        {
            deallocator (allocated[--allocatedCount]);
        }

        // Allocate one fourth of objects again to simulate situations when allocation happens after deallocation.
        while (allocatedCount < SAMPLE_SIZE / 2u + SAMPLE_SIZE / 4u)
        {
            allocated[allocatedCount++] = allocator ();
        }

        // Deallocate all objects left.
        while (allocatedCount > 0u)
        {
            deallocator (allocated[--allocatedCount]);
        }
    }
}

template <typename ObjectType>
static void AllocateDeallocate_NewDelete (benchmark::State &state)
{
    AllocateDeallocateRoutine <ObjectType> (
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
static void AllocateDeallocate_BoostObjectPool (benchmark::State &state)
{
    boost::object_pool <ObjectType> pool;
    AllocateDeallocateRoutine <ObjectType> (
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

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component1032b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component1032b);