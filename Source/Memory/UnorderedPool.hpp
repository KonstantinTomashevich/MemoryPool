#pragma once

#include <functional>

#include <Memory/Private/Commons.hpp>

namespace Memory
{
// TODO: Implement ordered (like boost) pools?
class UnorderedTrivialPool
{
public:
    using ValueType = void;

    UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize) noexcept;

    UnorderedTrivialPool (const UnorderedTrivialPool &other) = delete;

    UnorderedTrivialPool (UnorderedTrivialPool &&other) noexcept;

    ~UnorderedTrivialPool () noexcept;

    void *Acquire () noexcept;

    void Free (void *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

    SizeType GetPageCount () const;

    SizeType GetPageCapacity () const;

private:
    UntypedPoolFields fields_;
};

class UnorderedPool
{
public:
    using ValueType = void;

    using Constructor =  std::function <void (void *)>;
    using Destructor =  std::function <void (void *)>;

    UnorderedPool (SizeType pageCapacity, SizeType chunkSize,
                   Constructor constructor, Destructor destructor) noexcept;

    UnorderedPool (const UnorderedPool &other) = delete;

    UnorderedPool (UnorderedPool &&other) noexcept;

    ~UnorderedPool () noexcept;

    void *Acquire () noexcept;

    void Free (void *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

    SizeType GetPageCount () const;

    SizeType GetPageCapacity () const;

private:
    UntypedPoolFields fields_;
    Constructor constructor_;
    Destructor destructor_;
};
}