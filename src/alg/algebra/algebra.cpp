#include <alg/algebra.hpp>
#include <alg/errors.hpp>
// #include <alg/graph_validator.hpp>

#include <functional>
#include <initializer_list>
#include <numeric>
#include <variant>
#include <vector>

#include "alg/alphabet.hpp"
#include "alg/result.hpp"
#include "alg/symbol.hpp"
#include "alg/transition_graph.hpp"
#include "alg/word.hpp"
#include "algebra/word_adder.hpp"
#include "tables/addition_generation.hpp"
#include "tables/product_generation.hpp"
#include "tables/table_generator.hpp"
#include "transition/transition_manager.hpp"

namespace alg
{

struct Algebra::Impl
{
    using BinaryWordOp = OperationResult (Impl::*)(const Word &, const Word &) const;
    using StepFn = std::function<OperationResult(const Word &acc, const Symbol &z, size_t idx)>;

    Alphabet alphabet;
    Symbol zero;
    Symbol one;
    size_t word_size;

    CayleyTable addition_table;
    WordAdder adder;
    CayleyTable product_table;

    Impl(const TransitionManager &tm, Alphabet alpha, size_t size)
        : alphabet(std::move(alpha)), zero(tm.get_zero()), one(tm.next(zero).as_symbol()), word_size(size),
          addition_table(TableGenerator(tm, alphabet).generate(AdditionGeneration(tm, size))),
          adder(addition_table, zero),
          product_table(TableGenerator(tm, alphabet).generate(ProductGeneration(adder, zero, one, size)))
    {
    }

    OperationResult as_set(const Operand &operand)
    {
        if (std::holds_alternative<Word>(operand))
            return {std::get<Word>(operand)};

        return std::get<OperationResult>(operand);
    }

    void validate_same_size(const Word &x, const Word &y) const
    {
        if (x.size() != y.size())
            throw SizesMismatch("Words must be the same sizes");
    }

    void validate_non_zero(const Word &y) const
    {
        if (y.is_filled_with(zero))
            throw DivisionByZero("Cannot divide by zero word");
    }

    OperationResult apply_pairwise(const OperationResult &x, const OperationResult &y, BinaryWordOp op) const
    {
        OperationResult result;
        for (const auto &word_x : x)
            for (const auto &word_y : y)
                result.merge(std::invoke(op, this, word_x, word_y));
        return result;
    }

    OperationResult sum_partials(const std::vector<OperationResult> &partials, size_t size) const
    {
        OperationResult init = {Word(size, zero)};
        return std::accumulate(
            partials.begin(),
            partials.end(),
            init,
            [this](const OperationResult &acc, const OperationResult &part) { return adder.add(acc, part); }
        );
    }

    OperationResult multiply_digits(const Symbol &a, const Symbol &b, size_t shift, size_t size) const
    {
        OperationResult shifted;
        for (const auto &prod : product_table.get(a, b))
            shifted.emplace(prod.resized(size, zero).shifted_left(shift, zero));
        return shifted;
    }

    OperationResult multiply_by_symbol(const Word &x, const Symbol &s, size_t shift) const
    {
        std::vector<OperationResult> partials;
        for (size_t i = 0; i + shift < x.size(); ++i)
            partials.push_back(multiply_digits(x[i], s, i + shift, x.size()));
        return sum_partials(partials, x.size());
    }

    OperationResult multiply(const Word &x, const Word &y) const
    {
        validate_same_size(x, y);
        std::vector<OperationResult> partials;
        for (size_t i = 0; i < x.size(); ++i)
            partials.push_back(multiply_by_symbol(x, y[i], i));
        return sum_partials(partials, x.size());
    }

    OperationResult solve_step(const Word &target, const Word &acc, size_t idx, const StepFn &step_fn) const
    {
        if (idx == target.size())
            return {Word(target.size(), zero)};

        OperationResult result;
        for (const auto &z : alphabet)
        {
            for (const auto &next_acc : step_fn(acc, z, idx))
            {
                if (next_acc[idx] != target[idx])
                    continue;
                for (const auto &word : solve_step(target, next_acc, idx + 1, step_fn))
                    result.emplace(word.with_symbol(idx, z));
            }
        }
        return result;
    }

    OperationResult subtract(const Word &x, const Word &y) const
    {
        validate_same_size(x, y);
        return solve_step(
            x, y, 0, [this](const Word &acc, const Symbol &z, size_t idx) { return adder.add_digit(acc, z, idx); }
        );
    }

    OperationResult divide(const Word &x, const Word &y) const
    {
        validate_same_size(x, y);
        validate_non_zero(y);
        return solve_step(
            x,
            Word(x.size(), zero),
            0,
            [this, &y](const Word &acc, const Symbol &z, size_t idx)
            { return adder.add(OperationResult{acc}, multiply_by_symbol(y, z, idx)); }
        );
    }

    OperationResult subtract_no_overflow(const Word &x, const Word &y) const
    {
        const size_t n = x.size();
        const size_t ext_size = n + word_size;
        OperationResult result;

        for (const auto &z : subtract(x.resized(ext_size, zero), y.resized(ext_size, zero)))
            if (z.fits_in(n, zero))
                result.emplace(z.resized(n, zero));

        return result;
    }

    DivModResult divmod_step(const Word &x, const Word &y, std::set<Word> visited) const
    {
        visited.insert(x);

        OperationResult next_xs;
        for (const auto &candidate : subtract_no_overflow(x, y))
            if (!visited.contains(candidate))
                next_xs.emplace(candidate);

        if (next_xs.empty())
            return {{Word(x.size(), zero), x}};

        DivModResult result;
        for (const auto &next_x : next_xs)
            for (const auto &[q, r] : divmod_step(next_x, y, visited))
                for (const auto &next_q : adder.add(q, one))
                    result.emplace(next_q, r);

        return result;
    }

    DivModResult divmod(const Word &x, const Word &y) const
    {
        validate_same_size(x, y);
        validate_non_zero(y);
        return divmod_step(x, y, {});
    }

    OperationResult mod(const Word &x, const Word &y) const
    {
        OperationResult remainders;
        for (const auto &[q, r] : divmod(x, y))
            remainders.emplace(r);
        return remainders;
    }

    OperationResult div(const Word &x, const Word &y) const
    {
        OperationResult quotionts;
        for (const auto &[q, r] : divmod(x, y))
            quotionts.emplace(q);
        return quotionts;
    }
};

Algebra::Algebra(TransitionGraph graph, Alphabet alphabet, Symbol zero, size_t word_size)
{
    // GraphValidator::validate(graph, alphabet, zero, word_size);
    TransitionManager tm(std::move(graph), zero);
    m_impl = std::make_unique<Impl>(tm, std::move(alphabet), word_size);
}

Algebra::~Algebra() = default;
Algebra::Algebra(Algebra &&) noexcept = default;
Algebra &Algebra::operator=(Algebra &&) noexcept = default;

Word Algebra::zero_word() const
{
    return Word(m_impl->word_size, m_impl->zero);
}

Word Algebra::one_word() const
{
    return Word(m_impl->word_size, m_impl->zero, {m_impl->one});
}

Word Algebra::make_word(std::initializer_list<Symbol> symbols) const
{
    if (symbols.size() > m_impl->word_size)
        throw SizesMismatch("Word exceeds algebra word size");

    for (const auto &s : symbols)
        if (!m_impl->alphabet.contains(s))
            throw InvalidSymbol("Symbol does not belong to alphabet");

    return Word(m_impl->word_size, m_impl->zero, symbols);
}

Word Algebra::parse_word(std::string_view text) const
{
    if (text.size() > m_impl->word_size)
        throw SizesMismatch("Word exceeds algebra word size");

    Word word(m_impl->word_size, m_impl->zero);

    for (size_t i = 0; i < text.size(); i++)
    {
        Symbol s(text[i]);
        if (!m_impl->alphabet.contains(s))
            throw InvalidSymbol("Symbol does not belong to alphabet");

        word[i] = s;
    }

    return word;
}

OperationResult Algebra::add(const Operand &x, const Operand &y) const
{
    return m_impl->adder.add(m_impl->as_set(x), m_impl->as_set(y));
}

OperationResult Algebra::multiply(const Operand &x, const Operand &y) const
{
    return m_impl->apply_pairwise(m_impl->as_set(x), m_impl->as_set(y), &Impl::multiply);
}

OperationResult Algebra::subtract(const Operand &x, const Operand &y) const
{
    return m_impl->apply_pairwise(m_impl->as_set(x), m_impl->as_set(y), &Impl::subtract);
}

OperationResult Algebra::divide(const Operand &x, const Operand &y) const
{
    return m_impl->apply_pairwise(m_impl->as_set(x), m_impl->as_set(y), &Impl::divide);
}

DivModResult Algebra::divmod(const Operand &x, const Operand &y) const
{
    DivModResult result;
    for (const auto &word_x : m_impl->as_set(x))
        for (const auto &word_y : m_impl->as_set(y))
            result.merge(m_impl->divmod(word_x, word_y));
    return result;
}

OperationResult Algebra::div(const Operand &x, const Operand &y) const
{
    return m_impl->apply_pairwise(m_impl->as_set(x), m_impl->as_set(y), &Impl::div);
}

OperationResult Algebra::mod(const Operand &x, const Operand &y) const
{
    return m_impl->apply_pairwise(m_impl->as_set(x), m_impl->as_set(y), &Impl::mod);
}

} // namespace alg
