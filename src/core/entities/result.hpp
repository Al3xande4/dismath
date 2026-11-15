#pragma once

#include <set>

#include "symbol.hpp"
#include "word.hpp"

struct SymbolOperationResultItem
{
    Symbol value;
    Word carry;

    bool operator==(const SymbolOperationResultItem &) const = default;
    auto operator<=>(const SymbolOperationResultItem &) const = default;
};

using SymbolOperationResult = std::set<SymbolOperationResultItem>;

using OperationResult = std::set<Word>;
