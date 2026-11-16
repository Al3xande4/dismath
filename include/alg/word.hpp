#pragma once

#include <alg/errors.hpp>
#include <alg/symbol.hpp>

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace alg
{

class Word
{
private:
    size_t m_size;
    std::vector<Symbol> m_word;

public:
    Word(size_t size, Symbol fill) : m_size(size), m_word(size, fill) {}

    Word(size_t size, Symbol fill, std::initializer_list<Symbol> word) : m_size(size), m_word(size, fill)
    {
        if (word.size() > size)
            throw SizesMismatch("Word: word initializer list is bigger than actual size of Word");

        size_t i = 0;
        for (const auto &symbol : word)
            m_word[i++] = symbol;
    }

    Word(std::vector<Symbol> word) : m_size(word.size()), m_word(std::move(word)) {}

    size_t size() const { return m_size; }

    bool is_filled_with(const Symbol &fill) const;

    bool fits_in(size_t target_size, const Symbol &fill) const noexcept;

    Word shifted_left(size_t offset, const Symbol &fill) const;
    Word shifted_right(size_t offset, const Symbol &fill) const;

    Word with_symbol(size_t idx, const Symbol &symbol) const;

    Word resized(size_t new_size, const Symbol &fill) const;

    const Symbol &operator[](size_t idx) const { return m_word[idx]; }
    Symbol &operator[](size_t idx) { return m_word[idx]; }

    bool operator==(const Word &) const = default;
    auto operator<=>(const Word &) const = default;

    auto begin() const { return m_word.begin(); }
    auto end() const { return m_word.end(); }

    auto begin() { return m_word.begin(); }
    auto end() { return m_word.end(); }
};

} // namespace alg
