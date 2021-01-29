#pragma once

#include <functional>

#include <Memory/Private/Commons.hpp>

namespace Memory
{
// TODO: Implement ordered (like boost) pools?
class UnorderedTrivialPool
{
public:
    UnorderedTrivialPool (SizeType pageCapacity, SizeType chunkSize);

    ~UnorderedTrivialPool ();

    void *Acquire ();

    void Free (void *entry);

    void Shrink ();

    void Clean ();

private:
    UntypedPoolFields fields_;
};

class UnorderedPool
{
public:
    UnorderedPool (SizeType pageCapacity, SizeType chunkSize);

    ~UnorderedPool ();

    void *Acquire ();

    void Free (void *entry);

    void Shrink ();

    void Clean ();

private:
    UntypedPoolFields fields_;
    std::function <void (void *)> constructor_;
    std::function <void (void *)> destructor_;
};
}