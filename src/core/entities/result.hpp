#pragma once

#include <set>

#include "symbol.hpp"
#include "word.hpp"

struct OperationResultItem
{
    Symbol value;
    Word carry;
};

using OperationResult = std::set<OperationResultItem>;