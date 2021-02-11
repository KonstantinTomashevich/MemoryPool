#include <benchmark/benchmark.h>

#include "Adapters.hpp"
#include "DataTypes.hpp"

#define TEST_SAMPLE_SIZE 10000u

template <typename Pool>
void AllocateDeallocate (benchmark::State &state)
{
    std::array <typename Pool::EntryType *, TEST_SAMPLE_SIZE> allocated {};
    for (auto _ : state)
    {
        state.PauseTiming ();
        auto *pool = new Pool ();
        state.ResumeTiming ();

        // Start with filling full data sample.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; ++item)
        {
            allocated[item] = pool->Acquire ();
        }

        // Drop half of objects to measure deallocation speed. Deallocate
        // only even-index items to simulate random order deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            pool->Free (allocated[item]);
        }

        // Allocate one fourth of objects again to simulate situations when allocation happens after deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            allocated[item] = pool->Acquire ();
        }

        // Deallocate all objects left.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            pool->Free (allocated[item]);
        }

        for (std::size_t item = 1u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            pool->Free (allocated[item]);
        }

        state.PauseTiming ();
        delete pool;
        state.ResumeTiming ();
    }
}

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <Component32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <Component192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <Component1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, NewDeleteAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedBoostObjectPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedBoostObjectPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedBoostObjectPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedTrivialBoostObjectPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedTrivialBoostObjectPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, OrderedTrivialBoostObjectPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedBoostPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedBoostPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedBoostPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialBoostPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialBoostPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, UnorderedTrivialPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(AllocateDeallocate, TypedUnorderedTrivialPoolAdapter <TrivialComponent1032b>);