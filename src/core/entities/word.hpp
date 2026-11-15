#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <vector>

#include "core/api/errors.hpp"
#include "symbol.hpp"

class Word
{
private:
    size_t m_size;

    std::vector<Symbol> m_word;

public:
    Word(size_t size, Symbol fill) : m_size(size), m_word(size, (fill)) {}

    Word(size_t size, Symbol fill, std::initializer_list<Symbol> word) : m_size(size), m_word(size, (fill))
    {
        if (word.size() > size)
            throw SizesMismatch("Word: word initializer list is biggger than actual size of Word");

        size_t i = 0;
        for (const auto &symbol : word)
            m_word[i++] = symbol;
    }

    Word(std::vector<Symbol> word) : m_size(word.size()), m_word(std::move(word)) {}

    size_t size() const { return m_size; }

    bool is_filled_with(const Symbol &fill) const
    {
        return std::ranges::all_of(m_word, [&](const Symbol &s) { return s == fill; });
    }

    bool fits_in(size_t target_size, const Symbol &fill) const noexcept
    {
        if (target_size >= m_size)
            return true;

        return std::ranges::all_of(
            m_word.begin() + target_size, m_word.end(), [&fill](const Symbol &s) { return s == fill; }
        );
    }

    Word shifted_left(size_t offset, const Symbol &fill) const
    {
        Word shifted(m_size, fill);
        if (offset < m_size)
            std::ranges::copy_n(m_word.begin(), m_size - offset, shifted.m_word.begin() + offset);

        return shifted;
    }

    Word shifted_right(size_t offset, const Symbol &fill) const
    {
        Word shifted(m_size, fill);
        std::ranges::copy_n(m_word.begin() + offset, m_size - offset, shifted.m_word.begin());

        return shifted;
    }

    Word with_symbol(size_t idx, const Symbol &symbol) const
    {
        Word new_word(*this);
        new_word[idx] = symbol;
        return new_word;
    }

    Word resized(size_t new_size, const Symbol &fill) const
    {
        Word result(new_size, fill);
        std::ranges::copy_n(m_word.begin(), std::min(m_size, new_size), result.m_word.begin());
        return result;
    }

    const Symbol &operator[](size_t idx) const { return m_word[idx]; }
    Symbol &operator[](size_t idx) { return m_word[idx]; }

    bool operator==(const Word &) const = default;
    auto operator<=>(const Word &) const = default;

    auto begin() const { return m_word.begin(); }
    auto end() const { return m_word.end(); }

    auto begin() { return m_word.begin(); }
    auto end() { return m_word.end(); }
};