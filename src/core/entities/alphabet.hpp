#pragma once

#include <initializer_list>
#include <set>

#include "symbol.hpp"

class Alphabet
{
private:
    std::set<Symbol> m_alphabet;

public:
    Alphabet(std::initializer_list<Symbol> symbols)
    {
        for (const auto &symbol : symbols)
            m_alphabet.emplace(symbol);
    }

    Alphabet(std::set<Symbol> alphabet) : m_alphabet(std::move(alphabet)) {}

    bool contains(const Symbol &symbol) const { return m_alphabet.contains(symbol); }

    auto begin() { return m_alphabet.begin(); }
    auto end() { return m_alphabet.end(); }

    auto begin() const { return m_alphabet.begin(); }
    auto end() const { return m_alphabet.end(); }
};