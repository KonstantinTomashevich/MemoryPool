#include <cstdlib>

#include <benchmark/benchmark.h>

#include "Adapters.hpp"
#include "DataTypes.hpp"

#define DEALLOCATION_TEST_ITEM_COUNT 10000u

template <typename Pool>
void Deallocation (benchmark::State &state)
{
    std::array <typename Pool::EntryType *, DEALLOCATION_TEST_ITEM_COUNT> allocated {};
    for (auto _ : state)
    {
        state.PauseTiming ();
        auto *pool = new Pool ();

        for (std::size_t item = 0u; item < DEALLOCATION_TEST_ITEM_COUNT; ++item)
        {
            allocated[item] = pool->Acquire ();
        }

        // Instead of freeing item simultaneously, free even-index and odd-index items separately
        // to make benchmark closer to real situations where items are freed in random order.
        // TODO: Think again about this assumption. Is it really "random order" in most situations?
        state.ResumeTiming ();

        for (std::size_t item = 0u; item < DEALLOCATION_TEST_ITEM_COUNT; item += 2u)
        {
            pool->Free (allocated[item]);
        }

        for (std::size_t item = 1u; item < DEALLOCATION_TEST_ITEM_COUNT; item += 2u)
        {
            pool->Free (allocated[item]);
        }

        state.PauseTiming ();
        delete pool;
        state.ResumeTiming ();
    }
}

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <Component32b>);

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <Component192b>);

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Deallocation, NewDeleteAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedBoostObjectPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedBoostObjectPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedBoostObjectPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedTrivialBoostPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedTrivialBoostPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Deallocation, OrderedTrivialBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedBoostPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedBoostPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedBoostPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialBoostPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialBoostPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Deallocation, UnorderedTrivialPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Deallocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent1032b>);