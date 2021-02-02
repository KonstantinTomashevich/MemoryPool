#include <Memory/Private/Commons.hpp>

namespace Memory
{
BasePoolFields Memory::BasePoolFields::ForEmptyPool (SizeType pageCapacity)
{
    return {nullptr, nullptr, 0u, pageCapacity};
}

UntypedPoolFields UntypedPoolFields::ForEmptyPool (SizeType pageCapacity, SizeType chunkSize)
{
    return {nullptr, nullptr, 0u, pageCapacity, chunkSize};
}
}