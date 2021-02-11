#include <cstdlib>
#include <tuple>

#include <benchmark/benchmark.h>

#include "Adapters.hpp"
#include "DataTypes.hpp"

#define TEST_SAMPLE_SIZE 10000u

// std::tuple <Pools::EntryType...> construction doesn't work on clang-cl, therefore these helper methods were added.
template <typename Head, typename... Tail>
auto UnpackPoolEntryTypes ()
{
    if constexpr (sizeof...(Tail) == 0u)
    {
        return std::tuple <typename Head::EntryType *> (nullptr);
    }
    else
    {
        return std::tuple_cat (UnpackPoolEntryTypes <Head> (), UnpackPoolEntryTypes <Tail...> ());
    }
}

// In practical ECS, bunch of components are usually deleted at one shot (with entity
// deletion, for example). This template allows to benchmark suck situations.
template <typename... Pools>
void MixedAllocateDeallocate (benchmark::State &state)
{
    std::array <decltype (UnpackPoolEntryTypes <Pools...> ()), TEST_SAMPLE_SIZE> allocated;
    // Pools are created outside of benchmark to avoid pool destruction each iteration.
    // Pool destruction is slow operation for some pools and fresh start performance
    // is already tested in AllocateDeallocate benchmark.
    std::tuple <Pools...> pools {};

    for (auto _ : state)
    {
        // Start with filling full data sample.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; ++item)
        {
            std::apply ([&item, &allocated] (auto &... pool)
                        {
                            ((std::get <typename std::decay_t <decltype (pool)>::EntryType *> (allocated[item]) =
                                  pool.Acquire ()), ...);
                        }, pools);
        }

        // Drop half of objects to measure deallocation speed. Deallocate
        // only even-index items to simulate random order deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            std::apply ([&item, &allocated] (auto &... pool)
                        {
                            ((pool.Free (std::get <typename std::decay_t <decltype (pool)>::EntryType *> (
                                allocated[item]))), ...);
                        }, pools);
        }

        // Allocate one fourth of objects again to simulate situations when allocation happens after deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            std::apply ([&item, &allocated] (auto &... pool)
                        {
                            ((std::get <typename std::decay_t <decltype (pool)>::EntryType *> (allocated[item]) =
                                  pool.Acquire ()), ...);
                        }, pools);
        }

        // Deallocate all objects left.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            std::apply ([&item, &allocated] (auto &... pool)
                        {
                            ((pool.Free (std::get <typename std::decay_t <decltype (pool)>::EntryType *> (
                                allocated[item]))), ...);
                        }, pools);
        }

        for (std::size_t item = 1u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            std::apply ([&item, &allocated] (auto &... pool)
                        {
                            ((pool.Free (std::get <typename std::decay_t <decltype (pool)>::EntryType *> (
                                allocated[item]))), ...);
                        }, pools);
        }
    }
}

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   NewDeleteAdapter <Component32b>,
                   NewDeleteAdapter <Component192b>,
                   NewDeleteAdapter <Component1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   NewDeleteAdapter <TrivialComponent32b>,
                   NewDeleteAdapter <TrivialComponent192b>,
                   NewDeleteAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   OrderedBoostObjectPoolAdapter <Component32b>,
                   OrderedBoostObjectPoolAdapter <Component192b>,
                   OrderedBoostObjectPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   OrderedTrivialBoostPoolAdapter <TrivialComponent32b>,
                   OrderedTrivialBoostPoolAdapter <TrivialComponent192b>,
                   OrderedTrivialBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   UnorderedBoostPoolAdapter <Component32b>,
                   UnorderedBoostPoolAdapter <Component192b>,
                   UnorderedBoostPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   UnorderedTrivialBoostPoolAdapter <TrivialComponent32b>,
                   UnorderedTrivialBoostPoolAdapter <TrivialComponent192b>,
                   UnorderedTrivialBoostPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   UnorderedPoolAdapter <Component32b>,
                   UnorderedPoolAdapter <Component192b>,
                   UnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   UnorderedTrivialPoolAdapter <TrivialComponent32b>,
                   UnorderedTrivialPoolAdapter <TrivialComponent192b>,
                   UnorderedTrivialPoolAdapter <TrivialComponent1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   TypedUnorderedPoolAdapter <Component32b>,
                   TypedUnorderedPoolAdapter <Component192b>,
                   TypedUnorderedPoolAdapter <Component1032b>);

BENCHMARK_TEMPLATE(MixedAllocateDeallocate,
                   TypedUnorderedTrivialPoolAdapter <TrivialComponent32b>,
                   TypedUnorderedTrivialPoolAdapter <TrivialComponent192b>,
                   TypedUnorderedTrivialPoolAdapter <TrivialComponent1032b>);