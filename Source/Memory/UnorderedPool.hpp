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
    UnorderedPool (SizeType pageCapacity, SizeType chunkSize) noexcept;

    ~UnorderedPool () noexcept;

    void *Acquire () noexcept;

    void Free (void *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

private:
    UntypedPoolFields fields_;
    std::function <void (void *)> constructor_;
    std::function <void (void *)> destructor_;
};
}