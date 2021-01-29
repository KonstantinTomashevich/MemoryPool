#include <cassert>

#include <Memory/UnorderedPool.hpp>
#include <Memory/Private/PoolDetail.hpp>

namespace Memory
{
UnorderedTrivialPool::UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize)
    : fields_ {nullptr, nullptr, 0u, pageCapacity, chunkSize}
{
}

UnorderedTrivialPool::~UnorderedTrivialPool ()
{
    Clean ();
}

void *UnorderedTrivialPool::Acquire ()
{
    void *entry = PoolDetail::Acquire (fields_, fields_.chunkSize_);
    assert (entry);
    return entry;
}

void UnorderedTrivialPool::Free (void *entry)
{
    PoolDetail::Free (fields_, entry, fields_.chunkSize_);
}

void UnorderedTrivialPool::Shrink ()
{
    PoolDetail::Shrink (fields_, fields_.chunkSize_);
}

void UnorderedTrivialPool::Clean ()
{
    PoolDetail::TrivialClean (fields_, fields_.chunkSize_);
}

UnorderedPool::UnorderedPool (SizeType pageCapacity, SizeType chunkSize)
    : fields_ {nullptr, nullptr, 0u, pageCapacity, chunkSize}
{
}

UnorderedPool::~UnorderedPool ()
{
    Clean ();
}

void *UnorderedPool::Acquire ()
{
    void *entry = PoolDetail::Acquire (fields_, fields_.chunkSize_);
    assert (entry);
    constructor_ (entry);
    return entry;
}

void UnorderedPool::Free (void *entry)
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, fields_.chunkSize_);
    destructor_ (entry);
    PoolDetail::Free (fields_, entry, fields_.chunkSize_);
}

void UnorderedPool::Shrink ()
{
    PoolDetail::Shrink (fields_, fields_.chunkSize_);
}

void UnorderedPool::Clean ()
{
    PoolDetail::NonTrivialClean (fields_, fields_.chunkSize_, destructor_);
}
}
