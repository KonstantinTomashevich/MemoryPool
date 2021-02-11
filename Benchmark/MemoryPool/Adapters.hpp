#pragma once

#include <unordered_set>

#include <boost/pool/object_pool.hpp>

#include <Memory/UnorderedPool.hpp>
#include <Memory/TypedUnorderedPool.hpp>

#define MEMORY_LIBRARY_PAGE_CAPACITY 512u

template <typename ObjectType>
class NewDeleteAdapter
{
public:
    using EntryType = ObjectType;

    ~NewDeleteAdapter ();

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    // Set usage slightly affects benchmark results, but we can treat it as
    // real case scenario when created objects are actually stored somewhere.
    std::unordered_set <ObjectType *> allocated_ {};
};

template <typename ObjectType>
class OrderedBoostObjectPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    boost::object_pool <ObjectType> pool_;
};

template <typename ObjectType>
class OrderedTrivialBoostObjectPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    boost::pool <boost::default_user_allocator_malloc_free> pool_ {sizeof (ObjectType)};
};

template <typename ObjectType>
class UnorderedBoostPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    boost::pool <boost::default_user_allocator_malloc_free> pool_ {sizeof (ObjectType)};
};

template <typename ObjectType>
class UnorderedTrivialBoostPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    boost::pool <boost::default_user_allocator_malloc_free> pool_ {sizeof (ObjectType)};
};


template <typename ObjectType>
class UnorderedPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    static void Constructor (void *chunk) noexcept;

    static void Destructor (void *chunk) noexcept;

    Memory::UnorderedPool pool_ {MEMORY_LIBRARY_PAGE_CAPACITY, sizeof (ObjectType), Constructor, Destructor};
};

template <typename ObjectType>
class TypedUnorderedPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    Memory::TypedUnorderedPool <ObjectType> pool_ {MEMORY_LIBRARY_PAGE_CAPACITY};
};

template <typename ObjectType>
class UnorderedTrivialPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    Memory::UnorderedTrivialPool pool_ {MEMORY_LIBRARY_PAGE_CAPACITY, sizeof (ObjectType)};
};

template <typename ObjectType>
class TypedUnorderedTrivialPoolAdapter
{
public:
    using EntryType = ObjectType;

    ObjectType *Acquire ();

    void Free (ObjectType *object);

private:
    Memory::TypedUnorderedTrivialPool <ObjectType> pool_ {MEMORY_LIBRARY_PAGE_CAPACITY};
};

template <typename ObjectType>
NewDeleteAdapter <ObjectType>::~NewDeleteAdapter ()
{
    for (ObjectType *object : allocated_)
    {
        delete object;
    }
}

template <typename ObjectType>
ObjectType *NewDeleteAdapter <ObjectType>::Acquire ()
{
    auto *object = new ObjectType ();
    allocated_.emplace (object);
    return object;
}

template <typename ObjectType>
void NewDeleteAdapter <ObjectType>::Free (ObjectType *object)
{
    allocated_.erase (object);
    delete object;
}

template <typename ObjectType>
ObjectType *OrderedBoostObjectPoolAdapter <ObjectType>::Acquire ()
{
    return pool_.construct ();
}

template <typename ObjectType>
void OrderedBoostObjectPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.free (object);
}

template <typename ObjectType>
ObjectType *OrderedTrivialBoostObjectPoolAdapter <ObjectType>::Acquire ()
{
    return reinterpret_cast<ObjectType *> (pool_.ordered_malloc ());
}

template <typename ObjectType>
void OrderedTrivialBoostObjectPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.ordered_free (object);
}

template <typename ObjectType>
ObjectType *UnorderedBoostPoolAdapter <ObjectType>::Acquire ()
{
    return new (pool_.malloc ()) ObjectType ();
}

template <typename ObjectType>
void UnorderedBoostPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    object->~ObjectType ();
    pool_.free (object);
}

template <typename ObjectType>
ObjectType *UnorderedTrivialBoostPoolAdapter <ObjectType>::Acquire ()
{
    return reinterpret_cast<ObjectType *> (pool_.malloc ());
}

template <typename ObjectType>
void UnorderedTrivialBoostPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.free (object);
}

template <typename ObjectType>
ObjectType *UnorderedPoolAdapter <ObjectType>::Acquire ()
{
    return reinterpret_cast <ObjectType *> (pool_.Acquire ());
}

template <typename ObjectType>
void UnorderedPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.Free (object);
}

template <typename ObjectType>
void UnorderedPoolAdapter <ObjectType>::Constructor (void *chunk) noexcept
{
    new (chunk) ObjectType ();
}

template <typename ObjectType>
void UnorderedPoolAdapter <ObjectType>::Destructor (void *chunk) noexcept
{
    static_cast <ObjectType *> (chunk)->~ObjectType ();
}

template <typename ObjectType>
ObjectType *TypedUnorderedPoolAdapter <ObjectType>::Acquire ()
{
    return pool_.Acquire ();
}

template <typename ObjectType>
void TypedUnorderedPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.Free (object);
}

template <typename ObjectType>
ObjectType *UnorderedTrivialPoolAdapter <ObjectType>::Acquire ()
{
    return reinterpret_cast <ObjectType *> (pool_.Acquire ());
}

template <typename ObjectType>
void UnorderedTrivialPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.Free (object);
}

template <typename ObjectType>
ObjectType *TypedUnorderedTrivialPoolAdapter <ObjectType>::Acquire ()
{
    return pool_.Acquire ();
}

template <typename ObjectType>
void TypedUnorderedTrivialPoolAdapter <ObjectType>::Free (ObjectType *object)
{
    pool_.Free (object);
}

// TODO: Shrink and Clean operations benchmarks?