#include <cstdlib>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#include "Common.hpp"

#define POOL_MAX_SIZE 10000u

template <typename ObjectType, typename AllocatorLambda, typename DeallocatorLambda>
void DeallocationRoutine (benchmark::State &state, const AllocatorLambda &allocator,
                          const DeallocatorLambda &deallocator)
{
    std::array <ObjectType *, POOL_MAX_SIZE> allocated {};
    std::size_t allocatedCount = 0u;

    for (auto _ : state)
    {
        state.PauseTiming ();
        if (allocatedCount < POOL_MAX_SIZE / 10u)
        {
            while (allocatedCount < POOL_MAX_SIZE)
            {
                allocated[allocatedCount++] = allocator ();
            }
        }

        state.ResumeTiming ();
        deallocator (allocated[--allocatedCount]);
    }

    while (allocatedCount > 0u)
    {
        deallocator (allocated[--allocatedCount]);
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

template <typename ObjectType>
static void DeallocateOnly_UnorderedPool (benchmark::State &state)
{
    Memory::UnorderedPool pool = ConstructMemoryUnorderedPool <ObjectType> ();
    DeallocationRoutine <ObjectType> (
        state,
        [&pool] ()
        {
            return reinterpret_cast <ObjectType *> (pool.Acquire ());
        },
        [&pool] (ObjectType *object)
        {
            pool.Free (object);
        });
}

template <typename ObjectType>
static void DeallocateOnly_TypedUnorderedPool (benchmark::State &state)
{
    Memory::TypedUnorderedPool <ObjectType> pool {MEMORY_LIBRARY_PAGE_CAPACITY};
    DeallocationRoutine <ObjectType> (
        state,
        [&pool] ()
        {
            return pool.Acquire ();
        },
        [&pool] (ObjectType *object)
        {
            pool.Free (object);
        });
}

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component32b);

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component192b);

BENCHMARK_TEMPLATE(DeallocateOnly_NewDelete, Component1032b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component32b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component192b);

BENCHMARK_TEMPLATE(DeallocateOnly_BoostObjectPool, Component1032b);

BENCHMARK_TEMPLATE(DeallocateOnly_UnorderedPool, Component32b);

BENCHMARK_TEMPLATE(DeallocateOnly_UnorderedPool, Component192b);

BENCHMARK_TEMPLATE(DeallocateOnly_UnorderedPool, Component1032b);

BENCHMARK_TEMPLATE(DeallocateOnly_TypedUnorderedPool, Component32b);

BENCHMARK_TEMPLATE(DeallocateOnly_TypedUnorderedPool, Component192b);

BENCHMARK_TEMPLATE(DeallocateOnly_TypedUnorderedPool, Component1032b);