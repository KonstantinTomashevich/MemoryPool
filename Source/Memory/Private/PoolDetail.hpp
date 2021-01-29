#pragma once

#include <vector>

#include <Memory/Private/Commons.hpp>

namespace Memory
{
namespace PageDetail
{
ChunkPointer GetFirstChunk (PagePointer page);

ChunkPointer GetLastChunk (SizeType pageCapacity, SizeType chunkSize, ChunkPointer firstChunk);

bool IsFrom (PagePointer page, SizeType pageCapacity, SizeType chunkSize, ChunkPointer chunk);

ChunkPointer NextChunk (ChunkPointer current, SizeType chunkSize);

class PageIterator
{
public:
    PageIterator &operator ++ ();

    PagePointer operator * () const;

    bool operator == (const PageIterator &other) const;

    bool operator != (const PageIterator &other) const;

    static PageIterator Begin (BasePoolFields &poolFields);

    static PageIterator End (BasePoolFields &poolFields);

private:
    explicit PageIterator (PagePointer page);

    PagePointer currentPage_;
};
}

namespace PoolDetail
{
void AssertPoolState (BasePoolFields &fields, SizeType chunkSize);

void AssertFromPool (BasePoolFields &fields, void *entry, SizeType chunkSize);

void *Acquire (BasePoolFields &fields, SizeType chunkSize);

void Free (BasePoolFields &fields, void *entry, SizeType chunkSize);

void TrivialClean (BasePoolFields &fields, SizeType chunkSize);

// Template to help compiler optimize this method for typed pools.
template <typename Destructor>
void NonTrivialClean (BasePoolFields &fields, SizeType chunkSize, const Destructor &destructor);

void Shrink (BasePoolFields &fields, SizeType chunkSize);

ChunkPointer NextFreeChunk (ChunkPointer current);

PagePointer FindChunkPage (BasePoolFields &fields, SizeType chunkSize, ChunkPointer chunk, SizeType &pageIndexOutput);
}

namespace PoolDetail
{
template <typename Destructor>
void NonTrivialClean (BasePoolFields &fields, SizeType chunkSize, const Destructor &destructor)
{
    AssertPoolState (fields, chunkSize);
    // TODO: Static thread local is used to avoid vector allocation during this call. Rethink about this solution.
    static thread_local std::vector <bool> isChunkUsed {};

    isChunkUsed.resize (fields.pagesCount_ * fields.pageCapacity_);
    // TODO: Check shrink_to_fit performance foot print and think if it really makes things better.
    isChunkUsed.shrink_to_fit ();
    std::fill (isChunkUsed.begin (), isChunkUsed.end (), true);

    // Start from marking free chunks in isChunkUsed vector.
    {
        ChunkPointer freeChunk = fields.topFreeChunk_;
        while (freeChunk)
        {
            SizeType pageIndex;
            PagePointer page = FindChunkPage (fields, chunkSize, freeChunk, pageIndex);
            // TODO: Should situations where page is not found be processed?
            assert (page);
            assert (pageIndex < fields.pagesCount_);

            ChunkPointer pageFirstChunk = PageDetail::GetFirstChunk (page);
            assert (pageFirstChunk <= freeChunk);

            ptrdiff_t chunkIndex =
                (static_cast <uint8_t *> (freeChunk) - static_cast <uint8_t *> (pageFirstChunk)) / chunkSize;

            isChunkUsed[pageIndex * fields.pageCapacity_ + chunkIndex] = false;
            freeChunk = NextFreeChunk (freeChunk);
        }
    }

    // Call destructor for each used chunk.
    {
        size_t chunkGlobalIndex = 0u;
        PageDetail::PageIterator pageIterator = PageDetail::PageIterator::Begin (fields);
        const PageDetail::PageIterator pagesEnd = PageDetail::PageIterator::End (fields);

        while (pageIterator != pagesEnd)
        {
            PagePointer page = *pageIterator;
            ChunkPointer currentChunk = PageDetail::GetFirstChunk (page);
            ChunkPointer lastChunk = PageDetail::GetLastChunk (fields.pageCapacity_, chunkSize, currentChunk);

            while (currentChunk <= lastChunk)
            {
                if (isChunkUsed[chunkGlobalIndex])
                {
                    destructor (currentChunk);
                }

                currentChunk = PageDetail::NextChunk (currentChunk, chunkSize);
                ++chunkGlobalIndex;
            }

            ++pageIterator;
        }
    }

    // Now we can execute trivial clean, because all used chunks are destructed.
    TrivialClean (fields, chunkSize);
}
}
}