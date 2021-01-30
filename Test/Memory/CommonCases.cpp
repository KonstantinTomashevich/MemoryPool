#include "CommonCases.hpp"

bool NonTrivialData::operator == (const NonTrivialData &other) const
{
    return values_ == other.values_ &&
           first_ == other.first_ &&
           second_ == other.second_;
}

bool NonTrivialData::operator != (const NonTrivialData &other) const
{
    return !(other == *this);
}
