#pragma once

#include <cstddef>

#include "core/engine/rules/tables/cayley_table.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/word.hpp"

class WordAdder
{
private:
    const CayleyTable &m_addition_table;
    Symbol m_zero;

public:
    WordAdder(const CayleyTable &addition_table, Symbol zero)
        : m_addition_table(addition_table), m_zero(std::move(zero))
    {
    }

    OperationResult add(const Word &, const Symbol &) const;
    OperationResult add(const Word &, const Word &) const;
    OperationResult add(const OperationResult &, const OperationResult &) const;

    OperationResult add_digit(const Word &base, const Symbol &digit, size_t idx) const;
};