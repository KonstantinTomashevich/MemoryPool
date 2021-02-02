#include <cstdlib>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#include "Common.hpp"

#define TEST_SAMPLE_SIZE 10000u

template <typename ObjectType, typename AllocatorLambda, typename DeallocatorLambda>
void AllocateDeallocateRoutine (benchmark::State &state, const AllocatorLambda &allocator,
                                const DeallocatorLambda &deallocator)
{
    std::array <ObjectType *, TEST_SAMPLE_SIZE> allocated {};
    for (auto _ : state)
    {
        // Start with filling full data sample.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; ++item)
        {
            allocated[item] = allocator ();
        }

        // Drop half of objects to measure deallocation speed. Deallocate
        // only even-index items to simulate random order deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            deallocator (allocated[item]);
        }

        // Allocate one fourth of objects again to simulate situations when allocation happens after deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            allocated[item] = allocator ();
        }

        // Deallocate all objects left.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            deallocator (allocated[item]);
        }

        for (std::size_t item = 1u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            deallocator (allocated[item]);
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

template <typename ObjectType>
static void AllocateDeallocate_UnorderedPool (benchmark::State &state)
{
    Memory::UnorderedPool pool = ConstructMemoryUnorderedPool <ObjectType> ();
    AllocateDeallocateRoutine <ObjectType> (
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
static void AllocateDeallocate_TypedUnorderedPool (benchmark::State &state)
{
    Memory::TypedUnorderedPool <ObjectType> pool {MEMORY_LIBRARY_PAGE_CAPACITY};
    AllocateDeallocateRoutine <ObjectType> (
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

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_NewDelete, Component1032b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_BoostObjectPool, Component1032b);

BENCHMARK_TEMPLATE(AllocateDeallocate_UnorderedPool, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_UnorderedPool, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_UnorderedPool, Component1032b);

BENCHMARK_TEMPLATE(AllocateDeallocate_TypedUnorderedPool, Component32b);

BENCHMARK_TEMPLATE(AllocateDeallocate_TypedUnorderedPool, Component192b);

BENCHMARK_TEMPLATE(AllocateDeallocate_TypedUnorderedPool, Component1032b);