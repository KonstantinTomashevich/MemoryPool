#include <cstdlib>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#include "Common.hpp"

#define DEALLOCATION_TEST_ITEM_COUNT 10000u

template <typename ConstructorLambda, typename AllocatorLambda, typename DeallocatorLambda>
void DeallocationRoutine (benchmark::State &state, const ConstructorLambda &constructor,
                          const AllocatorLambda &allocator, const DeallocatorLambda &deallocator)
{
    std::array <void *, DEALLOCATION_TEST_ITEM_COUNT> allocated {};
    for (auto _ : state)
    {
        state.PauseTiming ();
        auto *pool = constructor ();

        for (std::size_t item = 0u; item < DEALLOCATION_TEST_ITEM_COUNT; ++item)
        {
            allocated[item] = allocator (pool);
        }

        // Instead of freeing item simultaneously, free even-index and odd-index items separately
        // to make benchmark closer to real situations where items are freed in random order.
        state.ResumeTiming ();

        for (std::size_t item = 0u; item < DEALLOCATION_TEST_ITEM_COUNT; item += 2u)
        {
            deallocator (pool, allocated[item]);
        }

        for (std::size_t item = 1u; item < DEALLOCATION_TEST_ITEM_COUNT; item += 2u)
        {
            deallocator (pool, allocated[item]);
        }

        state.PauseTiming ();
        delete pool;
        state.ResumeTiming ();
    }
}

template <typename ObjectType>
static void DeallocateOnly_NewDelete (benchmark::State &state)
{
    using PlaceholderType = std::size_t;
    DeallocationRoutine (
        state,
        [] () -> PlaceholderType *
        {
            return nullptr;
        },
        [] (PlaceholderType *pool)
        {
            return new ObjectType ();
        },
        [] (PlaceholderType *pool, void *object)
        {
            delete reinterpret_cast <ObjectType *> (object);
        });
}

template <typename ObjectType>
static void DeallocateOnly_BoostObjectPool (benchmark::State &state)
{
    DeallocationRoutine (
        state,
        [] ()
        {
            return new boost::object_pool <ObjectType> ();
        },
        [] (boost::object_pool <ObjectType> *pool)
        {
            return pool->construct ();
        },
        [] (boost::object_pool <ObjectType> *pool, void *object)
        {
            pool->free (reinterpret_cast<ObjectType *>(object));
        });
}

template <typename ObjectType>
static void DeallocateOnly_UnorderedPool (benchmark::State &state)
{
    DeallocationRoutine (
        state,
        [] ()
        {
            return NewMemoryUnorderedPool <ObjectType> ();
        },
        [] (Memory::UnorderedPool *pool)
        {
            return pool->Acquire ();
        },
        [] (Memory::UnorderedPool *pool, void *object)
        {
            pool->Free (object);
        });
}

template <typename ObjectType>
static void DeallocateOnly_TypedUnorderedPool (benchmark::State &state)
{
    DeallocationRoutine (
        state,
        [] ()
        {
            return new Memory::TypedUnorderedPool <ObjectType> (MEMORY_LIBRARY_PAGE_CAPACITY);
        },
        [] (Memory::TypedUnorderedPool <ObjectType> *pool)
        {
            return pool->Acquire ();
        },
        [] (Memory::TypedUnorderedPool <ObjectType> *pool, void *object)
        {
            pool->Free (reinterpret_cast<ObjectType *>(object));
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