#include "algebra.hpp"

#include <functional>
#include <numeric>
#include <vector>

#include "core/api/errors.hpp"
#include "core/engine/rules/tables/generation/addition_generation.hpp"
#include "core/engine/rules/tables/generation/product_generation.hpp"
#include "core/engine/rules/tables/table_generator.hpp"
#include "core/entities/result.hpp"
#include "core/entities/word.hpp"
#include "word_adder.hpp"

Algebra::Algebra(const TransitionManager &transition, const Alphabet &alphabet, size_t word_size)
    : m_alphabet(alphabet), m_zero(transition.get_zero()), m_one(transition.next(m_zero).as_symbol()),
      m_word_size(word_size),
      m_addition_table(TableGenerator(transition, alphabet).generate(AdditionGeneration(transition, word_size))),
      m_adder(m_addition_table, m_zero),
      m_product_table(
          TableGenerator(transition, alphabet).generate(ProductGeneration(m_adder, m_zero, m_one, word_size))
      )
{
}

void Algebra::_validate_same_size(const Word &x, const Word &y) const
{
    if (x.size() != y.size())
        throw SizesMismatch("Words must be the same sizes");
}

void Algebra::_validate_non_zero(const Word &y) const
{
    if (y.is_filled_with(m_zero))
        throw DivisionByZero("Cannot divide by zero word");
}

OperationResult Algebra::_apply_pairwise(const OperationResult &x, const OperationResult &y, BinaryWordOp op) const
{
    OperationResult result;

    for (const auto &word_x : x)
        for (const auto &word_y : y)
            result.merge(std::invoke(op, this, word_x, word_y));

    return result;
}

OperationResult Algebra::_sum_partials(const std::vector<OperationResult> &partials) const
{
    OperationResult init = {Word(partials[0].begin()->size(), m_zero)};

    return std::accumulate(
        partials.begin(),
        partials.end(),
        init,
        [this](const OperationResult &acc, const OperationResult &part) { return m_adder.add(acc, part); }
    );
}

OperationResult Algebra::add(const Word &x, const Word &y) const
{
    return m_adder.add(x, y);
}

OperationResult Algebra::add(const OperationResult &x, const OperationResult &y) const
{
    return m_adder.add(x, y);
}

OperationResult Algebra::_multiply_digits(const Symbol &a, const Symbol &b, size_t shift, size_t word_size) const
{
    OperationResult shifted_products;

    for (const auto &prod : m_product_table.get(a, b))
        shifted_products.emplace(prod.resized(word_size, m_zero).shifted_left(shift, m_zero));

    return shifted_products;
}

OperationResult Algebra::_multiply_by_symbol(const Word &x, const Symbol &s, size_t shift) const
{
    std::vector<OperationResult> partials;

    for (size_t i = 0; i + shift < x.size(); ++i)
        partials.push_back(_multiply_digits(x[i], s, i + shift, x.size()));

    return _sum_partials(partials);
}

OperationResult Algebra::multiply(const Word &x, const Word &y) const
{
    _validate_same_size(x, y);

    std::vector<OperationResult> partials;

    for (size_t i = 0; i < x.size(); ++i)
        partials.push_back(_multiply_by_symbol(x, y[i], i));

    return _sum_partials(partials);
}

OperationResult Algebra::multiply(const OperationResult &x, const OperationResult &y) const
{
    return _apply_pairwise(x, y, &Algebra::multiply);
}

OperationResult Algebra::_solve_step(const Word &target, const Word &acc, size_t idx, const StepFn &step_fn) const
{
    if (idx == target.size())
        return {Word(target.size(), m_zero)};

    OperationResult result;

    for (const auto &z : m_alphabet)
    {
        for (const auto &next_acc : step_fn(acc, z, idx))
        {
            if (next_acc[idx] != target[idx])
                continue;

            for (const auto &word : _solve_step(target, next_acc, idx + 1, step_fn))
                result.emplace(word.with_symbol(idx, z));
        }
    }

    return result;
}

OperationResult Algebra::subtract(const Word &x, const Word &y) const
{
    _validate_same_size(x, y);

    return _solve_step(
        x,
        y,
        0,
        [this](const Word &acc, const Symbol &z, size_t idx) -> OperationResult
        { return m_adder.add_digit(acc, z, idx); }
    );
}

OperationResult Algebra::subtract(const OperationResult &x, const OperationResult &y) const
{
    return _apply_pairwise(x, y, &Algebra::subtract);
}

OperationResult Algebra::divide(const Word &x, const Word &y) const
{
    _validate_same_size(x, y);
    _validate_non_zero(y);

    return _solve_step(
        x,
        Word(x.size(), m_zero),
        0,
        [this, &y](const Word &acc, const Symbol &z, size_t idx)
        { return m_adder.add(OperationResult{acc}, _multiply_by_symbol(y, z, idx)); }
    );
}

OperationResult Algebra::divide(const OperationResult &x, const OperationResult &y) const
{
    return _apply_pairwise(x, y, &Algebra::divide);
}

OperationResult Algebra::_subtract_no_overflow(const Word &x, const Word &y) const
{
    const size_t n = x.size();
    const size_t ext_size = n + m_word_size;
    OperationResult result;

    for (const auto &z : subtract(x.resized(ext_size, m_zero), y.resized(ext_size, m_zero)))
        if (z.fits_in(n, m_zero))
            result.emplace(z.resized(n, m_zero));

    return result;
}

DivModResult Algebra::_divmod_step(const Word &x, const Word &y) const
{
    OperationResult next_xs = _subtract_no_overflow(x, y);
    if (next_xs.empty())
        return {{Word(x.size(), m_zero), x}};

    DivModResult result;
    const Word one(x.size(), m_zero, {m_one});

    for (const auto &next_x : next_xs)
        for (const auto &[q, r] : _divmod_step(next_x, y))
            for (const auto &next_q : m_adder.add(q, one))
                result.emplace(next_q, r);

    return result;
}

DivModResult Algebra::divmod(const Word &x, const Word &y) const
{
    _validate_same_size(x, y);
    _validate_non_zero(y);

    return _divmod_step(x, y);
}

OperationResult Algebra::div(const Word &x, const Word &y) const
{
    OperationResult quotients;
    for (const auto &[q, r] : divmod(x, y))
        quotients.emplace(q);

    return quotients;
}

OperationResult Algebra::div(const OperationResult &x, const OperationResult &y) const
{
    return _apply_pairwise(x, y, &Algebra::div);
}

OperationResult Algebra::mod(const Word &x, const Word &y) const
{
    OperationResult remainders;
    for (const auto &[q, r] : divmod(x, y))
        remainders.emplace(r);

    return remainders;
}

OperationResult Algebra::mod(const OperationResult &x, const OperationResult &y) const
{
    return _apply_pairwise(x, y, &Algebra::mod);
}