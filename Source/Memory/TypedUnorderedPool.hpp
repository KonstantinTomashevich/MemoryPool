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
    explicit TypedUnorderedTrivialPool (SizeType pageCapacity) noexcept;

    ~TypedUnorderedTrivialPool () noexcept;

    Entry *Acquire () noexcept;

    void Free (Entry *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

private:
    BasePoolFields fields_;
};

template <typename Entry>
void EntryDefaultConstructor (Entry *entry) noexcept;

template <typename Entry>
void EntryDefaultDestructor (Entry *entry) noexcept;

template <
    typename Entry,
    void (*Constructor) (Entry *) noexcept = EntryDefaultConstructor,
    void (*Destructor) (Entry *) noexcept = EntryDefaultDestructor>
class TypedUnorderedPool
{
    static_assert (sizeof (Entry) >= sizeof (uintptr_t),
                   "Entry type size must be at equal or greater than pointer size!");

    static_assert (Constructor);
    static_assert (Destructor);

public:
    explicit TypedUnorderedPool (SizeType pageCapacity) noexcept;

    ~TypedUnorderedPool () noexcept;

    Entry *Acquire () noexcept;

    void Free (Entry *entry) noexcept;

    void Shrink () noexcept;

    void Clean () noexcept;

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
    : fields_ {nullptr, nullptr, 0u, pageCapacity}
{
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

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
TypedUnorderedPool <Entry, Constructor, Destructor>::TypedUnorderedPool (SizeType pageCapacity) noexcept
    : fields_ {nullptr, nullptr, 0u, pageCapacity}
{
}

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
TypedUnorderedPool <Entry, Constructor, Destructor>::~TypedUnorderedPool () noexcept
{
    Clean ();
}

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
Entry *TypedUnorderedPool <Entry, Constructor, Destructor>::Acquire () noexcept
{
    auto *entry = reinterpret_cast <Entry *> (PoolDetail::Acquire (fields_, sizeof (Entry)));
    assert (entry);
    Constructor (entry);
    return entry;
}

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Free (Entry *entry) noexcept
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, sizeof (Entry));
    Destructor (entry);
    PoolDetail::Free (fields_, entry, sizeof (Entry));
}

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Shrink () noexcept
{
    PoolDetail::Shrink (fields_, sizeof (Entry));
}

template <typename Entry, void (*Constructor) (Entry *) noexcept, void (*Destructor) (Entry *) noexcept>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Clean () noexcept
{
    PoolDetail::NonTrivialClean (
        fields_, sizeof (Entry),
        [] (void *entry)
        {
            Destructor (reinterpret_cast <Entry *> (entry));
        });
}
}