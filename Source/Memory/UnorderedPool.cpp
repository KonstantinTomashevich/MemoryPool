#include <cassert>

#include <Memory/UnorderedPool.hpp>
#include <Memory/Private/PoolDetail.hpp>

namespace Memory
{
UnorderedTrivialPool::UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize) noexcept
    : fields_ {nullptr, nullptr, 0u, pageCapacity, chunkSize}
{
}

UnorderedTrivialPool::~UnorderedTrivialPool () noexcept
{
    Clean ();
}

void *UnorderedTrivialPool::Acquire () noexcept
{
    void *entry = PoolDetail::Acquire (fields_, fields_.chunkSize_);
    assert (entry);
    return entry;
}

void UnorderedTrivialPool::Free (void *entry) noexcept
{
    PoolDetail::Free (fields_, entry, fields_.chunkSize_);
}

void UnorderedTrivialPool::Shrink () noexcept
{
    PoolDetail::Shrink (fields_, fields_.chunkSize_);
}

void UnorderedTrivialPool::Clean () noexcept
{
    PoolDetail::TrivialClean (fields_, fields_.chunkSize_);
}

UnorderedPool::UnorderedPool (SizeType pageCapacity, SizeType chunkSize) noexcept
    : fields_ {nullptr, nullptr, 0u, pageCapacity, chunkSize}
{
}

UnorderedPool::~UnorderedPool () noexcept
{
    Clean ();
}

void *UnorderedPool::Acquire () noexcept
{
    void *entry = PoolDetail::Acquire (fields_, fields_.chunkSize_);
    assert (entry);
    constructor_ (entry);
    return entry;
}

void UnorderedPool::Free (void *entry) noexcept
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, fields_.chunkSize_);
    destructor_ (entry);
    PoolDetail::Free (fields_, entry, fields_.chunkSize_);
}

void UnorderedPool::Shrink () noexcept
{
    PoolDetail::Shrink (fields_, fields_.chunkSize_);
}

void UnorderedPool::Clean () noexcept
{
    PoolDetail::NonTrivialClean (fields_, fields_.chunkSize_, destructor_);
}
}
