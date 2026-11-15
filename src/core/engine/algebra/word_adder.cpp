#include "word_adder.hpp"

#include <cstddef>

#include "core/api/errors.hpp"
#include "core/entities/result.hpp"

OperationResult WordAdder::_add_digit(const Word &base, const Symbol &digit, size_t idx) const
{
    OperationResult result;

    for (const auto &res : m_addition_table.get(base[idx], digit))
    {
        Word next_word = base;
        next_word[idx] = res[0];

        Word carry = res.shifted_left(idx, m_zero);
        carry[idx] = m_zero;

        result.merge(add(next_word, carry));
    }

    return result;
}

OperationResult WordAdder::add(const Word &word, const Symbol &symbol) const
{
    return _add_digit(word, symbol, 0);
}

OperationResult WordAdder::add(const Word &x, const Word &y) const
{
    if (x.size() != y.size())
        throw SizesMismatch("Words must be the same sizes to add them");

    if (x.is_filled_with(m_zero))
        return {y};
    if (y.is_filled_with(m_zero))
        return {x};

    OperationResult result = {x};
    for (size_t i = 0; i < x.size(); i++)
    {
        OperationResult next;
        for (const auto &word : result)
            next.merge(_add_digit(word, y[i], i));

        result = std::move(next);
        if (result.empty())
            break;
    }

    return result;
}

OperationResult WordAdder::add(const OperationResult &x, const OperationResult &y) const
{
    OperationResult result;

    for (const auto &word_x : x)
        for (const auto &word_y : y)
            result.merge(add(word_x, word_y));

    return result;
}