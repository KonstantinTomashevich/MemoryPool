// C++ algorithms are required only for heavy-duty debug assertions.
#ifndef NDEBUG

#include <algorithm>

#endif

#include <cassert>
#include <cstdlib>
#include <vector>

#include <Memory/Private/PoolDetail.hpp>

namespace Memory
{
namespace PoolDetail
{
void SetNextFreeChunk (ChunkPointer chunk, ChunkPointer next) noexcept;

void PushFreeChunk (BasePoolFields &fields, ChunkPointer chunk) noexcept;

ChunkPointer PopFreeChunk (BasePoolFields &fields) noexcept;

void PushPage (BasePoolFields &fields, PagePointer page) noexcept;

void PopPage (BasePoolFields &fields, PagePointer page, PagePointer previous, PagePointer next) noexcept;
}

namespace PageDetail
{
PagePointer ConstructEmptyPage (SizeType pageCapacity, SizeType chunkSize) noexcept;

PagePointer NextPage (PagePointer current) noexcept;

void SetNextPage (PagePointer page, PagePointer next) noexcept;
}

namespace PoolDetail
{
void AssertPoolState (BasePoolFields &fields, SizeType chunkSize) noexcept
{
    assert (chunkSize >= sizeof (uintptr_t));
    assert (fields.pageCapacity_ > 0u);
    assert (!fields.topFreeChunk_ || fields.topPage_);

    assert (std::count_if (
        PageDetail::PageIterator::Begin (fields),
        PageDetail::PageIterator::End (fields),
        [] (PagePointer page)
        {
            return true;
        }) == fields.pagesCount_);
}

void AssertFromPool (BasePoolFields &fields, void *entry, SizeType chunkSize) noexcept
{
    assert (std::find_if (
        PageDetail::PageIterator::Begin (fields),
        PageDetail::PageIterator::End (fields),

        [entry, &fields, chunkSize] (PagePointer page)
        {
            return PageDetail::IsFrom (page, fields.pageCapacity_, chunkSize, entry);
        }) != PageDetail::PageIterator::End (fields));
}

void *Acquire (BasePoolFields &fields, SizeType chunkSize) noexcept
{
    AssertPoolState (fields, chunkSize);
    if (!fields.topFreeChunk_)
    {
        PagePointer newPage = PageDetail::ConstructEmptyPage (fields.pageCapacity_, chunkSize);
        ChunkPointer first = PageDetail::GetFirstChunk (newPage);
        ChunkPointer last = PageDetail::GetLastChunk (fields.pageCapacity_, chunkSize, first);

        *static_cast <uintptr_t *> (last) = reinterpret_cast <uintptr_t> (fields.topFreeChunk_);
        fields.topFreeChunk_ = first;
        PushPage (fields, newPage);
        assert (fields.topFreeChunk_);
    }

    return PopFreeChunk (fields);
}

void Free (BasePoolFields &fields, void *entry, SizeType chunkSize) noexcept
{
    AssertPoolState (fields, chunkSize);
    AssertFromPool (fields, entry, chunkSize);
    PushFreeChunk (fields, entry);
}

void TrivialClean (BasePoolFields &fields, SizeType chunkSize) noexcept
{
    AssertPoolState (fields, chunkSize);
    PageDetail::PageIterator iterator = PageDetail::PageIterator::Begin (fields);
    PageDetail::PageIterator end = PageDetail::PageIterator::End (fields);

    while (iterator != end)
    {
        PagePointer page = *iterator;
        ++iterator;
        free (page);
    }

    fields.topFreeChunk_ = nullptr;
    fields.topPage_ = nullptr;
}

void Shrink (BasePoolFields &fields, SizeType chunkSize) noexcept
{
    AssertPoolState (fields, chunkSize);
    // TODO: Static thread local is used to avoid vector allocation during this call. Rethink about this solution.
    static thread_local std::vector <SizeType> freeChunkCounts {};

    freeChunkCounts.resize (fields.pagesCount_);
    // TODO: Check shrink_to_fit performance foot print and think if it really makes things better.
    freeChunkCounts.shrink_to_fit ();
    std::fill (freeChunkCounts.begin (), freeChunkCounts.end (), 0u);

    ChunkPointer freeChunk = fields.topFreeChunk_;
    const PageDetail::PageIterator pagesEnd = PageDetail::PageIterator::End (fields);

    // Count free chunks on each page.
    while (freeChunk)
    {
        SizeType pageIndex;
        PagePointer page = FindChunkPage (fields, chunkSize, freeChunk, pageIndex);
        // TODO: Should situations where page is not found be processed?
        assert (page);
        assert (pageIndex < fields.pagesCount_);

        ++freeChunkCounts[pageIndex];
        freeChunk = NextFreeChunk (freeChunk);
    }

    // Erase from list free chunks of empty pages.
    {
        ChunkPointer previous = nullptr;
        freeChunk = fields.topFreeChunk_;

        while (freeChunk)
        {
            SizeType pageIndex;
            PagePointer page = FindChunkPage (fields, chunkSize, freeChunk, pageIndex);
            // TODO: Should situations where page is not found be processed?
            assert (page);
            assert (pageIndex < fields.pagesCount_);

            assert (freeChunkCounts[pageIndex] <= fields.pageCapacity_);
            ChunkPointer next = NextFreeChunk (freeChunk);

            if (freeChunkCounts[pageIndex] == fields.pageCapacity_)
            {
                if (previous)
                {
                    SetNextFreeChunk (previous, next);
                }
                else
                {
                    fields.topFreeChunk_ = next;
                }
            }
            else
            {
                previous = freeChunk;
            }

            freeChunk = next;
        }
    }

    // Erase empty pages.
    {
        SizeType pageIndex = 0u;
        PagePointer previousPage = nullptr;
        PageDetail::PageIterator pageIterator = PageDetail::PageIterator::Begin (fields);

        while (pageIterator != pagesEnd)
        {
            PagePointer currentPage = *pageIterator;
            ++pageIterator;
            assert (freeChunkCounts[pageIndex] <= fields.pageCapacity_);

            if (freeChunkCounts[pageIndex] == fields.pageCapacity_)
            {
                PopPage (fields, currentPage, previousPage, *pageIterator);
            }
            else
            {
                previousPage = currentPage;
            }

            ++pageIndex;
        }
    }
}

ChunkPointer NextFreeChunk (ChunkPointer current) noexcept
{
    assert (current);
    return reinterpret_cast <ChunkPointer> (*static_cast <uintptr_t *> (current));
}

PagePointer FindChunkPage (BasePoolFields &fields, SizeType chunkSize,
                           ChunkPointer chunk, SizeType &pageIndexOutput) noexcept
{
    pageIndexOutput = 0u;
    PageDetail::PageIterator pageIterator = PageDetail::PageIterator::Begin (fields);
    const PageDetail::PageIterator pagesEnd = PageDetail::PageIterator::End (fields);

    while (pageIterator != pagesEnd)
    {
        if (PageDetail::IsFrom (*pageIterator, fields.pageCapacity_, chunkSize, chunk))
        {
            return *pageIterator;
        }
        else
        {
            ++pageIterator;
            ++pageIndexOutput;
        }
    }

    return nullptr;
}

void SetNextFreeChunk (ChunkPointer chunk, ChunkPointer next) noexcept
{
    *static_cast <uintptr_t *> (chunk) = reinterpret_cast <uintptr_t> (next);
}

void PushFreeChunk (BasePoolFields &fields, ChunkPointer chunk) noexcept
{
    SetNextFreeChunk (chunk, fields.topFreeChunk_);
    fields.topFreeChunk_ = chunk;
}

ChunkPointer PopFreeChunk (BasePoolFields &fields) noexcept
{
    assert (fields.topFreeChunk_);
    ChunkPointer current = fields.topFreeChunk_;
    fields.topFreeChunk_ = NextFreeChunk (current);
    return current;
}

void PushPage (BasePoolFields &fields, PagePointer page) noexcept
{
    PageDetail::SetNextPage (page, fields.topPage_);
    fields.topPage_ = page;
    ++fields.pagesCount_;
}

void PopPage (BasePoolFields &fields, PagePointer page, PagePointer previous, PagePointer next) noexcept
{
    assert (fields.pagesCount_);
    assert (PageDetail::NextPage (page) == next);
    assert (PageDetail::NextPage (previous) == page);

    free (page);
    --fields.pagesCount_;

    if (previous)
    {
        PageDetail::SetNextPage (previous, next);
    }
    else
    {
        fields.topPage_ = next;
    }
}
}

namespace PageDetail
{
ChunkPointer GetFirstChunk (PagePointer page) noexcept
{
    return static_cast <ChunkPointer> (static_cast <uintptr_t *> (page) + 1u);
}

ChunkPointer GetLastChunk (SizeType pageCapacity, SizeType chunkSize, ChunkPointer firstChunk) noexcept
{
    return static_cast <ChunkPointer> (static_cast <uint8_t *> (firstChunk) + (pageCapacity - 1u) * chunkSize);
}

bool IsFrom (PagePointer page, SizeType pageCapacity, SizeType chunkSize, ChunkPointer chunk) noexcept
{
    assert (page);
    void *firstChunk = static_cast <void *> (static_cast <uintptr_t *> (page) + 1u);
    void *lastChunk = static_cast <void *> (static_cast <uint8_t *> (firstChunk) + (pageCapacity - 1u) * chunkSize);
    return chunk >= firstChunk && chunk <= lastChunk;
}

ChunkPointer NextChunk (ChunkPointer current, SizeType chunkSize) noexcept
{
    assert (current);
    return reinterpret_cast <ChunkPointer> (static_cast <uint8_t *> (current) + chunkSize);
}

PageIterator &PageIterator::operator ++ () noexcept
{
    assert (currentPage_);
    currentPage_ = NextPage (currentPage_);
    return *this;
}

PagePointer PageIterator::operator * () const noexcept
{
    return currentPage_;
}

bool PageIterator::operator == (const PageIterator &other) const noexcept
{
    return currentPage_ == other.currentPage_;
}

bool PageIterator::operator != (const PageIterator &other) const noexcept
{
    return !(*this == other);
}

PageIterator PageIterator::Begin (BasePoolFields &poolFields) noexcept
{
    return PageIterator (poolFields.topPage_);
}

PageIterator PageIterator::End (BasePoolFields &poolFields) noexcept
{
    return PageIterator (nullptr);
}

PageIterator::PageIterator (PagePointer page) noexcept
    : currentPage_ (page)
{
}

PagePointer ConstructEmptyPage (SizeType pageCapacity, SizeType chunkSize) noexcept
{
    assert (pageCapacity > 0u);
    assert (chunkSize >= sizeof (uintptr_t));

    // TODO: Handle malloc errors?
    PagePointer page = malloc (sizeof (uintptr_t) + pageCapacity * chunkSize);
    assert (page);

    ChunkPointer previous = GetFirstChunk (page);
    ChunkPointer current = NextChunk (previous, chunkSize);
    ChunkPointer last = GetLastChunk (pageCapacity, chunkSize, previous);

    while (current <= last)
    {
        *static_cast <uintptr_t *> (previous) = reinterpret_cast <uintptr_t> (current);
        previous = current;
        current = NextChunk (current, chunkSize);
    }

    return page;
}

PagePointer NextPage (PagePointer current) noexcept
{
    assert (current);
    return reinterpret_cast <PagePointer> (*static_cast <uintptr_t *> (current));
}

void SetNextPage (PagePointer page, PagePointer next) noexcept
{
    *static_cast <uintptr_t *> (page) = reinterpret_cast <uintptr_t> (next);
}
}
}
