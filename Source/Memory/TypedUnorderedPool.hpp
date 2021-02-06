#pragma once

#include <cassert>
#include <type_traits>

#include <Memory/Private/Commons.hpp>
#include <Memory/Private/PoolDetail.hpp>

namespace Memory
{
template <typename Entry>
class TypedUnorderedTrivialPool
{
    static_assert (std::is_trivial_v <Entry>);
    static_assert (sizeof (Entry) >= sizeof (uintptr_t),
                   "Entry type size must be at equal or greater than pointer size!");

public:
    using ValueType = Entry;

    explicit TypedUnorderedTrivialPool (SizeType pageCapacity) noexcept;

    TypedUnorderedTrivialPool (const TypedUnorderedTrivialPool &other) = delete;

    TypedUnorderedTrivialPool (TypedUnorderedTrivialPool &&other) noexcept;

    ~TypedUnorderedTrivialPool () noexcept;

    Entry *Acquire () noexcept;

    void Free (Entry *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

    SizeType GetPageCount () const;

    SizeType GetPageCapacity () const;

private:
    BasePoolFields fields_;
};

template <typename Entry>
using PoolEntryOperation = void (*) (Entry *) noexcept;

template <typename Entry>
void EntryDefaultConstructor (Entry *entry) noexcept;

template <typename Entry>
void EntryDefaultDestructor (Entry *entry) noexcept;

template <
    typename Entry,
    PoolEntryOperation <Entry> Constructor = EntryDefaultConstructor,
    PoolEntryOperation <Entry> Destructor = EntryDefaultDestructor>
class TypedUnorderedPool
{
    static_assert (sizeof (Entry) >= sizeof (uintptr_t),
                   "Entry type size must be at equal or greater than pointer size!");

    static_assert (!std::is_trivial_v <Entry> ||
                   // Cast is required because of bug in some GCC versions, which forbids such equality checks.
                   Constructor != static_cast <PoolEntryOperation <Entry>> (EntryDefaultConstructor <Entry>) ||
                   Destructor != static_cast <PoolEntryOperation <Entry>> (EntryDefaultDestructor <Entry>),
                   "There is no practical sense in using trivial type with default constructor "
                   "and destructor, consider using TypedUnorderedTrivialPool instead.");

    static_assert (Constructor);
    static_assert (Destructor);

public:
    using ValueType = Entry;

    explicit TypedUnorderedPool (SizeType pageCapacity) noexcept;

    TypedUnorderedPool (const TypedUnorderedPool &other) = delete;

    TypedUnorderedPool (TypedUnorderedPool &&other) noexcept;

    ~TypedUnorderedPool () noexcept;

    Entry *Acquire () noexcept;

    void Free (Entry *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

    SizeType GetPageCount () const;

    SizeType GetPageCapacity () const;

private:
    BasePoolFields fields_;
};

template <typename Entry>
void EntryDefaultConstructor (Entry *entry) noexcept
{
    assert (entry);
    new (entry) Entry ();
}


template <typename Entry>
void EntryDefaultDestructor (Entry *entry) noexcept
{
    assert (entry);
    entry->~Entry ();
}

template <typename Entry>
TypedUnorderedTrivialPool <Entry>::TypedUnorderedTrivialPool (SizeType pageCapacity) noexcept
    : fields_ (BasePoolFields::ForEmptyPool (pageCapacity))
{
}

template <typename Entry>
TypedUnorderedTrivialPool <Entry>::TypedUnorderedTrivialPool (TypedUnorderedTrivialPool &&other) noexcept
    : fields_ (other.fields_)
{
    other.fields_ = BasePoolFields::ForEmptyPool (fields_.pageCapacity_);
}

template <typename Entry>
TypedUnorderedTrivialPool <Entry>::~TypedUnorderedTrivialPool () noexcept
{
    Clean ();
}

template <typename Entry>
Entry *TypedUnorderedTrivialPool <Entry>::Acquire () noexcept
{
    auto *entry = reinterpret_cast <Entry *> (PoolDetail::Acquire (fields_, sizeof (Entry)));
    assert (entry);
    return entry;
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Free (Entry *entry) noexcept
{
    PoolDetail::Free (fields_, entry, sizeof (Entry));
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Shrink () noexcept
{
    PoolDetail::Shrink (fields_, sizeof (Entry));
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Clean () noexcept
{
    PoolDetail::TrivialClean (fields_, sizeof (Entry));
}

template <typename Entry>
SizeType TypedUnorderedTrivialPool <Entry>::GetPageCount () const
{
    return fields_.pageCount_;
}

template <typename Entry>
SizeType TypedUnorderedTrivialPool <Entry>::GetPageCapacity () const
{
    return fields_.pageCapacity_;
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
TypedUnorderedPool <Entry, Constructor, Destructor>::TypedUnorderedPool (SizeType pageCapacity) noexcept
    : fields_ {nullptr, nullptr, 0u, pageCapacity}
{
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
TypedUnorderedPool <Entry, Constructor, Destructor>::TypedUnorderedPool (TypedUnorderedPool &&other) noexcept
    : fields_ (other.fields_)
{
    other.fields_ = BasePoolFields::ForEmptyPool (fields_.pageCapacity_);
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
TypedUnorderedPool <Entry, Constructor, Destructor>::~TypedUnorderedPool () noexcept
{
    Clean ();
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
Entry *TypedUnorderedPool <Entry, Constructor, Destructor>::Acquire () noexcept
{
    auto *entry = reinterpret_cast <Entry *> (PoolDetail::Acquire (fields_, sizeof (Entry)));
    assert (entry);
    Constructor (entry);
    return entry;
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Free (Entry *entry) noexcept
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, sizeof (Entry));
    Destructor (entry);
    PoolDetail::Free (fields_, entry, sizeof (Entry));
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Shrink () noexcept
{
    PoolDetail::Shrink (fields_, sizeof (Entry));
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Clean () noexcept
{
    PoolDetail::NonTrivialClean (
        fields_, sizeof (Entry),
        [] (void *entry)
        {
            Destructor (reinterpret_cast <Entry *> (entry));
        });
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
SizeType TypedUnorderedPool <Entry, Constructor, Destructor>::GetPageCount () const
{
    return fields_.pageCount_;
}

template <typename Entry, PoolEntryOperation <Entry> Constructor, PoolEntryOperation <Entry> Destructor>
SizeType TypedUnorderedPool <Entry, Constructor, Destructor>::GetPageCapacity () const
{
    return fields_.pageCapacity_;
}
}