#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#include "Common.hpp"

#define ALLOCATION_TEST_ITEM_COUNT 10000u

template <typename ConstructorLambda, typename AllocatorLambda>
void AllocationRoutine (benchmark::State &state,
                        const ConstructorLambda &constructor,
                        const AllocatorLambda &allocator)
{
    for (auto _ : state)
    {
        state.PauseTiming ();
        auto *pool = constructor ();
        state.ResumeTiming ();

        for (std::size_t item = 0; item < ALLOCATION_TEST_ITEM_COUNT; ++item)
        {
            auto *object = allocator (pool);
            benchmark::DoNotOptimize (object);
        }

        state.PauseTiming ();
        delete pool;
        state.ResumeTiming ();
    }
}

template <typename ObjectType>
static void AllocateOnly_NewDelete (benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming ();
        std::vector <ObjectType *> allocated;
        state.ResumeTiming ();

        for (std::size_t item = 0; item < ALLOCATION_TEST_ITEM_COUNT; ++item)
        {
            auto *object = new ObjectType ();
            state.PauseTiming ();
            allocated.push_back (object);
            state.ResumeTiming ();
        }

        state.PauseTiming ();
        for (ObjectType *object : allocated)
        {
            delete object;
        }

        state.ResumeTiming ();
    }
}

template <typename ObjectType>
static void AllocateOnly_BoostObjectPool (benchmark::State &state)
{
    AllocationRoutine (
        state,
        [] ()
        {
            return new boost::object_pool <ObjectType> ();
        },
        [] (boost::object_pool <ObjectType> *pool)
        {
            return pool->construct ();
        });
}

template <typename ObjectType>
static void AllocateOnly_UnorderedPool (benchmark::State &state)
{
    AllocationRoutine (
        state,
        [] ()
        {
            return NewMemoryUnorderedPool <ObjectType> ();
        },
        [] (Memory::UnorderedPool *pool)
        {
            return pool->Acquire ();
        });
}

template <typename ObjectType>
static void AllocateOnly_TypedUnorderedPool (benchmark::State &state)
{
    AllocationRoutine (
        state,
        [] ()
        {
            return new Memory::TypedUnorderedPool <ObjectType> (MEMORY_LIBRARY_PAGE_CAPACITY);
        },
        [] (Memory::TypedUnorderedPool <ObjectType> *pool)
        {
            return pool->Acquire ();
        });
}

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_NewDelete, Component1032b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_BoostObjectPool, Component1032b);

BENCHMARK_TEMPLATE(AllocateOnly_UnorderedPool, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_UnorderedPool, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_UnorderedPool, Component1032b);

BENCHMARK_TEMPLATE(AllocateOnly_TypedUnorderedPool, Component32b);

BENCHMARK_TEMPLATE(AllocateOnly_TypedUnorderedPool, Component192b);

BENCHMARK_TEMPLATE(AllocateOnly_TypedUnorderedPool, Component1032b);