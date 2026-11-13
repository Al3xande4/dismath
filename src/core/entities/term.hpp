#pragma once

#include <initializer_list>
#include <set>

#include "symbol.hpp"

class Term
{
private:
    std::set<Symbol> m_symbols;

public:
    Term(Symbol symbol) : m_symbols{symbol} {}

    Term(std::initializer_list<Symbol> symbols)
    {
        for (const auto &symbol : symbols)
            m_symbols.emplace(symbol);
    }

    auto begin() { return m_symbols.begin(); }
    auto end() { return m_symbols.end(); }

    auto begin() const { return m_symbols.begin(); }
    auto end() const { return m_symbols.end(); }
};