#include <cassert>

#include <Memory/UnorderedPool.hpp>
#include <Memory/Private/PoolDetail.hpp>

namespace Memory
{
UnorderedTrivialPool::UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize) noexcept
    : fields_ (UntypedPoolFields::ForEmptyPool (pageCapacity, chunkSize))
{
}

UnorderedTrivialPool::UnorderedTrivialPool (UnorderedTrivialPool &&other) noexcept
    : fields_ (other.fields_)
{
    other.fields_ = UntypedPoolFields::ForEmptyPool (fields_.pageCapacity_, fields_.chunkSize_);
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

SizeType UnorderedTrivialPool::GetPageCount () const
{
    return fields_.pageCount_;
}

SizeType UnorderedTrivialPool::GetPageCapacity () const
{
    return fields_.pageCapacity_;
}

UnorderedPool::UnorderedPool (SizeType pageCapacity, SizeType chunkSize,
                              Constructor constructor, Destructor destructor) noexcept
    : fields_ {nullptr, nullptr, 0u, pageCapacity, chunkSize},
      constructor_ (constructor),
      destructor_ (destructor)
{
    assert (constructor_);
    assert (destructor_);
}

UnorderedPool::UnorderedPool (UnorderedPool &&other) noexcept
    : fields_ (other.fields_),
      constructor_ (other.constructor_),
      destructor_ (other.destructor_)
{
    other.fields_ = UntypedPoolFields::ForEmptyPool (fields_.pageCapacity_, fields_.chunkSize_);
    assert (constructor_);
    assert (destructor_);
}

UnorderedPool::~UnorderedPool () noexcept
{
    Clean ();
}

void *UnorderedPool::Acquire () noexcept
{
    void *entry = PoolDetail::Acquire (fields_, fields_.chunkSize_);
    assert (entry);
    assert (constructor_);

    constructor_ (entry);
    return entry;
}

void UnorderedPool::Free (void *entry) noexcept
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, fields_.chunkSize_);

    assert (destructor_);
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

SizeType UnorderedPool::GetPageCount () const
{
    return fields_.pageCount_;
}

SizeType UnorderedPool::GetPageCapacity () const
{
    return fields_.pageCapacity_;
}
}
