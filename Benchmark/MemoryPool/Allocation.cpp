#include <benchmark/benchmark.h>

#include "Adapters.hpp"
#include "DataTypes.hpp"

#define ALLOCATION_TEST_ITEM_COUNT 10000u

template <typename Pool>
void Allocation (benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming ();
        auto *pool = new Pool ();
        state.ResumeTiming ();

        for (std::size_t item = 0u; item < ALLOCATION_TEST_ITEM_COUNT; ++item)
        {
            auto *object = pool->Acquire ();
            benchmark::DoNotOptimize (object);
        }

        state.PauseTiming ();
        delete pool;
        state.ResumeTiming ();
    }
}

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <Component32b>);

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <Component192b>);

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Allocation, NewDeleteAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Allocation, OrderedBoostObjectPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Allocation, UnorderedTrivialPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedPoolAdapter <Component32b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedPoolAdapter <Component192b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent32b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent192b>);

BENCHMARK_TEMPLATE(Allocation, TypedUnorderedTrivialPoolAdapter <TrivialComponent1032b>);