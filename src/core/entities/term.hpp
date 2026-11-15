#pragma once

#include <initializer_list>
#include <set>

#include "symbol.hpp"

class Term
{
private:
    std::set<Symbol> m_symbols;

public:
    Term() = default;

    Term(Symbol symbol) : m_symbols{symbol} {}

    Term(std::set<Symbol> symbols) : m_symbols(std::move(symbols)) {}

    Term(std::initializer_list<Symbol> symbols)
    {
        for (const auto &symbol : symbols)
            m_symbols.emplace(symbol);
    }

    Symbol as_symbol() const { return *m_symbols.begin(); }

    bool operator==(const Term &) const = default;
    auto operator<=>(const Term &) const = default;

    size_t size() const { return m_symbols.size(); }

    bool contains(const Symbol &symbol) const { return m_symbols.contains(symbol); }

    auto begin() { return m_symbols.begin(); }
    auto end() { return m_symbols.end(); }

    auto begin() const { return m_symbols.begin(); }
    auto end() const { return m_symbols.end(); }

    void merge(Term term) { m_symbols.merge(term.m_symbols); }
};