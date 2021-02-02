#include <cstdlib>
#include <tuple>

#include <benchmark/benchmark.h>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#include "Common.hpp"

#define TEST_SAMPLE_SIZE 10000u

template <typename ObjectType, typename Allocator, typename Deallocator>
struct ManagedObjectTypeDescriptor
{
    struct RuntimeType
    {
        std::array <ObjectType *, TEST_SAMPLE_SIZE> objects_;
        Allocator allocator_;
        Deallocator deallocator_;
    };

    Allocator allocator_;
    Deallocator deallocator_;

    RuntimeType ConstructRuntime ()
    {
        return {{}, allocator_, deallocator_};
    }
};

// std::tuple <Descriptors::RuntimeType...> construction doesn't
// work on clang-cl, therefore these helper methods were added.
template <typename Head>
auto UnpackDescriptors (Head &head)
{
    return std::tuple <typename Head::RuntimeType> (head.ConstructRuntime ());
}

template <typename Head, typename... Tail>
auto UnpackDescriptors (Head &head, Tail &... tail)
{
    return std::tuple_cat (UnpackDescriptors (head), UnpackDescriptors (tail...));
}

// In practical ECS, bunch of components are usually deleted at one shot (with entity
// deletion, for example). This template allows to benchmark suck situations.
template <typename... Descriptors>
void AllocateDeallocateRoutine (benchmark::State &state, Descriptors... descriptors)
{
    auto allocated = UnpackDescriptors (descriptors...);
    for (auto _ : state)
    {
        // Start with filling full data sample.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; ++item)
        {
            std::apply ([&item] (auto &... parts)
                        {
                            ((parts.objects_[item] = parts.allocator_ ()), ...);
                        }, allocated);
        }

        // Drop half of objects to measure deallocation speed. Deallocate
        // only even-index items to simulate random order deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            std::apply ([&item] (auto &... parts)
                        {
                            ((parts.deallocator_ (parts.objects_[item])), ...);
                        }, allocated);
        }

        // Allocate one fourth of objects again to simulate situations when allocation happens after deallocation.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            std::apply ([&item] (auto &... parts)
                        {
                            ((parts.objects_[item] = parts.allocator_ ()), ...);
                        }, allocated);
        }

        // Deallocate all objects left.
        for (std::size_t item = 0u; item < TEST_SAMPLE_SIZE / 2u; item += 2u)
        {
            std::apply ([&item] (auto &... parts)
                        {
                            ((parts.deallocator_ (parts.objects_[item])), ...);
                        }, allocated);
        }

        for (std::size_t item = 1u; item < TEST_SAMPLE_SIZE; item += 2u)
        {
            std::apply ([&item] (auto &... parts)
                        {
                            ((parts.deallocator_ (parts.objects_[item])), ...);
                        }, allocated);
        }
    }
}

static void MixedAllocateDeallocate_NewDelete (benchmark::State &state)
{
    auto construct32b = [] ()
    {
        return new Component32b ();
    };

    auto destroy32b = [] (Component32b *object)
    {
        delete object;
    };

    auto construct192b = [] ()
    {
        return new Component192b ();
    };

    auto destroy192b = [] (Component192b *object)
    {
        delete object;
    };

    auto construct1032b = [] ()
    {
        return new Component1032b ();
    };

    auto destroy1032b = [] (Component1032b *object)
    {
        delete object;
    };

    AllocateDeallocateRoutine (
        state,
        ManagedObjectTypeDescriptor <Component32b, decltype (construct32b), decltype (destroy32b)>
            {construct32b, destroy32b},
        ManagedObjectTypeDescriptor <Component192b, decltype (construct192b), decltype (destroy192b)>
            {construct192b, destroy192b},
        ManagedObjectTypeDescriptor <Component1032b, decltype (construct1032b), decltype (destroy1032b)>
            {construct1032b, destroy1032b});
}

static void MixedAllocateDeallocate_BoostObjectPool (benchmark::State &state)
{
    boost::object_pool <Component32b> pool32b;
    boost::object_pool <Component192b> pool192b;
    boost::object_pool <Component1032b> pool1032b;

    auto construct32b = [&pool32b] ()
    {
        return pool32b.construct ();
    };

    auto destroy32b = [&pool32b] (Component32b *object)
    {
        pool32b.destroy (object);
    };

    auto construct192b = [&pool192b] ()
    {
        return pool192b.construct ();
    };

    auto destroy192b = [&pool192b] (Component192b *object)
    {
        pool192b.destroy (object);
    };

    auto construct1032b = [&pool1032b] ()
    {
        return pool1032b.construct ();
    };

    auto destroy1032b = [&pool1032b] (Component1032b *object)
    {
        pool1032b.destroy (object);
    };

    AllocateDeallocateRoutine (
        state,
        ManagedObjectTypeDescriptor <Component32b, decltype (construct32b), decltype (destroy32b)>
            {construct32b, destroy32b},
        ManagedObjectTypeDescriptor <Component192b, decltype (construct192b), decltype (destroy192b)>
            {construct192b, destroy192b},
        ManagedObjectTypeDescriptor <Component1032b, decltype (construct1032b), decltype (destroy1032b)>
            {construct1032b, destroy1032b});
}

static void MixedAllocateDeallocate_UnorderedPool (benchmark::State &state)
{
    Memory::UnorderedPool pool32b = ConstructMemoryUnorderedPool <Component32b> ();
    Memory::UnorderedPool pool192b = ConstructMemoryUnorderedPool <Component192b> ();
    Memory::UnorderedPool pool1032b = ConstructMemoryUnorderedPool <Component1032b> ();

    auto construct32b = [&pool32b] ()
    {
        return reinterpret_cast <Component32b *> (pool32b.Acquire ());
    };

    auto destroy32b = [&pool32b] (Component32b *object)
    {
        pool32b.Free (object);
    };

    auto construct192b = [&pool192b] ()
    {
        return reinterpret_cast <Component192b *> (pool192b.Acquire ());
    };

    auto destroy192b = [&pool192b] (Component192b *object)
    {
        pool192b.Free (object);
    };

    auto construct1032b = [&pool1032b] ()
    {
        return reinterpret_cast <Component1032b *> (pool1032b.Acquire ());
    };

    auto destroy1032b = [&pool1032b] (Component1032b *object)
    {
        pool1032b.Free (object);
    };

    AllocateDeallocateRoutine (
        state,
        ManagedObjectTypeDescriptor <Component32b, decltype (construct32b), decltype (destroy32b)>
            {construct32b, destroy32b},
        ManagedObjectTypeDescriptor <Component192b, decltype (construct192b), decltype (destroy192b)>
            {construct192b, destroy192b},
        ManagedObjectTypeDescriptor <Component1032b, decltype (construct1032b), decltype (destroy1032b)>
            {construct1032b, destroy1032b});
}

static void MixedAllocateDeallocate_TypedUnorderedPool (benchmark::State &state)
{
    Memory::TypedUnorderedPool <Component32b> pool32b {MEMORY_LIBRARY_PAGE_CAPACITY};
    Memory::TypedUnorderedPool <Component192b> pool192b {MEMORY_LIBRARY_PAGE_CAPACITY};
    Memory::TypedUnorderedPool <Component1032b> pool1032b {MEMORY_LIBRARY_PAGE_CAPACITY};

    auto construct32b = [&pool32b] ()
    {
        return pool32b.Acquire ();
    };

    auto destroy32b = [&pool32b] (Component32b *object)
    {
        pool32b.Free (object);
    };

    auto construct192b = [&pool192b] ()
    {
        return pool192b.Acquire ();
    };

    auto destroy192b = [&pool192b] (Component192b *object)
    {
        pool192b.Free (object);
    };

    auto construct1032b = [&pool1032b] ()
    {
        return pool1032b.Acquire ();
    };

    auto destroy1032b = [&pool1032b] (Component1032b *object)
    {
        pool1032b.Free (object);
    };

    AllocateDeallocateRoutine (
        state,
        ManagedObjectTypeDescriptor <Component32b, decltype (construct32b), decltype (destroy32b)>
            {construct32b, destroy32b},
        ManagedObjectTypeDescriptor <Component192b, decltype (construct192b), decltype (destroy192b)>
            {construct192b, destroy192b},
        ManagedObjectTypeDescriptor <Component1032b, decltype (construct1032b), decltype (destroy1032b)>
            {construct1032b, destroy1032b});
}

// Min time is required, because these benchmarks are quite unstable if count of iterations is too small.
BENCHMARK(MixedAllocateDeallocate_NewDelete)->MinTime (15.0);

BENCHMARK(MixedAllocateDeallocate_BoostObjectPool)->MinTime (15.0);

BENCHMARK(MixedAllocateDeallocate_UnorderedPool)->MinTime (15.0);

BENCHMARK(MixedAllocateDeallocate_TypedUnorderedPool)->MinTime (15.0);