#pragma once

#include <functional>

#include <Memory/Private/Commons.hpp>

namespace Memory
{
// TODO: Implement ordered (like boost) pools?
class UnorderedTrivialPool
{
public:
    UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize) noexcept;

    ~UnorderedTrivialPool () noexcept;

    void *Acquire () noexcept;

    void Free (void *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

private:
    UntypedPoolFields fields_;
};

class UnorderedPool
{
public:
    using Constructor =  std::function <void (void *)>;
    using Destructor =  std::function <void (void *)>;

    UnorderedPool (SizeType pageCapacity, SizeType chunkSize,
                   Constructor constructor, Destructor destructor) noexcept;

    ~UnorderedPool () noexcept;

    void *Acquire () noexcept;

    void Free (void *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

private:
    UntypedPoolFields fields_;
    Constructor constructor_;
    Destructor destructor_;
};
}