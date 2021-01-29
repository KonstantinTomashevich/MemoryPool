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
    static_assert (std::is_trivial_v <Entry> ());
    static_assert (sizeof (Entry) >= sizeof (uintptr_t),
                   "Entry type size must be at equal or greater than pointer size!");

public:
    explicit TypedUnorderedTrivialPool (SizeType pageCapacity);

    explicit ~TypedUnorderedTrivialPool ();

    Entry *Acquire ();

    void Free (Entry *entry);

    void Shrink ();

    void Clean ();

private:
    BasePoolFields fields_;
};

template <typename Entry>
void EntryDefaultConstructor (Entry *entry);

template <typename Entry>
void EntryDefaultDestructor (Entry *entry);

template <
    typename Entry,
    void (*Constructor) (Entry *) = EntryDefaultConstructor,
    void (*Destructor) (Entry *) = EntryDefaultDestructor>
class TypedUnorderedPool
{
    static_assert (sizeof (Entry) >= sizeof (uintptr_t),
                   "Entry type size must be at equal or greater than pointer size!");

public:
    explicit TypedUnorderedPool (SizeType pageCapacity);

    explicit ~TypedUnorderedPool ();

    Entry *Acquire ();

    void Free (Entry *entry);

    void Shrink ();

    void Clean ();

private:
    BasePoolFields fields_;
};

template <typename Entry>
void EntryDefaultConstructor (Entry *entry)
{
    assert (entry);
    new (entry) Entry ();
}


template <typename Entry>
void EntryDefaultDestructor (Entry *entry)
{
    assert (entry);
    delete entry;
}

template <typename Entry>
TypedUnorderedTrivialPool <Entry>::TypedUnorderedTrivialPool (SizeType pageCapacity)
    : fields_ {nullptr, nullptr, 0u, pageCapacity}
{
}

template <typename Entry>
TypedUnorderedTrivialPool <Entry>::~TypedUnorderedTrivialPool ()
{
    Clean ();
}

template <typename Entry>
Entry *TypedUnorderedTrivialPool <Entry>::Acquire ()
{
    auto *entry = reinterpret_cast <Entry *> (PoolDetail::Acquire (fields_, sizeof (Entry)));
    assert (entry);
    return entry;
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Free (Entry *entry)
{
    PoolDetail::Free (fields_, entry, sizeof (Entry));
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Shrink ()
{
    PoolDetail::Shrink (fields_, sizeof (Entry));
}

template <typename Entry>
void TypedUnorderedTrivialPool <Entry>::Clean ()
{
    PoolDetail::TrivialClean (fields_, sizeof (Entry));
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
TypedUnorderedPool <Entry, Constructor, Destructor>::TypedUnorderedPool (SizeType pageCapacity)
    : fields_ {nullptr, nullptr, 0u, pageCapacity}
{
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
TypedUnorderedPool <Entry, Constructor, Destructor>::~TypedUnorderedPool ()
{
    Clean ();
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
Entry *TypedUnorderedPool <Entry, Constructor, Destructor>::Acquire ()
{
    auto *entry = reinterpret_cast <Entry *> (PoolDetail::Acquire (fields_, sizeof (Entry)));
    assert (entry);
    Constructor (entry);
    return entry;
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Free (Entry *entry)
{
    assert (entry);
    PoolDetail::AssertFromPool (fields_, entry, sizeof (Entry));
    Destructor (entry);
    PoolDetail::Free (fields_, entry, sizeof (Entry));
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Shrink ()
{
    PoolDetail::Shrink (fields_, sizeof (Entry));
}

template <typename Entry, void (*Constructor) (Entry *), void (*Destructor) (Entry *)>
void TypedUnorderedPool <Entry, Constructor, Destructor>::Clean ()
{
    PoolDetail::NonTrivialClean (
        fields_, sizeof (Entry),
        [] (void *entry)
        {
            Destructor (reinterpret_cast <Entry *> entry);
        });
}
}