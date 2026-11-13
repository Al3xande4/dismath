#pragma once

#include <cstddef>
#include <vector>

#include "symbol.hpp"

class Word
{
private:
    size_t m_size;

    std::vector<Symbol> m_word;

public:
    Word(size_t size, Symbol fill) : m_size(size), m_word(size, (fill)) {}

    Word(std::vector<Symbol> word) : m_size(word.size()), m_word(std::move(word)) {}

    size_t get_size() const { return m_size; }

    const Symbol &operator[](size_t idx) const { return m_word[idx]; }
    Symbol &operator[](size_t idx) { return m_word[idx]; }

    bool operator==(const Word &) const = default;
    auto operator<=>(const Word &) const = default;
};