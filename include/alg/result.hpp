#pragma once

#include <alg/symbol.hpp>
#include <alg/word.hpp>

#include <set>

namespace alg
{

struct SymbolOperationResultItem
{
    Symbol value;
    Word carry;

    bool operator==(const SymbolOperationResultItem &) const = default;
    auto operator<=>(const SymbolOperationResultItem &) const = default;
};

using SymbolOperationResult = std::set<SymbolOperationResultItem>;

using OperationResult = std::set<Word>;

struct DivisionResult
{
    Word quotient;
    Word remainder;

    bool operator==(const DivisionResult &) const = default;
    auto operator<=>(const DivisionResult &) const = default;
};

using DivModResult = std::set<DivisionResult>;

} // namespace alg
