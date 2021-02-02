#pragma once

#include <cstdint>

namespace Memory
{
// Typically, we don't need pools with huge entries (over 4 GB) or pages,
// so 32-bit size type usage saves some memory.
using SizeType = uint32_t;

using PagePointer = void *;
using ChunkPointer = void *;

struct BasePoolFields
{
    static BasePoolFields ForEmptyPool (SizeType pageCapacity);

    ChunkPointer topFreeChunk_ = nullptr;
    PagePointer topPage_ = nullptr;
    SizeType pageCount_ = 0u;
    SizeType pageCapacity_ = 0u;
};

struct UntypedPoolFields : public BasePoolFields
{
    static UntypedPoolFields ForEmptyPool (SizeType pageCapacity, SizeType chunkSize);

    SizeType chunkSize_ = 0u;
};
}