#pragma once

#include <alg/symbol.hpp>

#include <initializer_list>
#include <set>
#include <utility>

namespace alg
{

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

    size_t size() const { return m_alphabet.size(); }

    bool empty() const { return m_alphabet.empty(); }

    bool contains(const Symbol &symbol) const { return m_alphabet.contains(symbol); }

    bool operator==(const Alphabet &) const = default;

    auto begin() { return m_alphabet.begin(); }
    auto end() { return m_alphabet.end(); }

    auto begin() const { return m_alphabet.begin(); }
    auto end() const { return m_alphabet.end(); }
};

} // namespace alg
