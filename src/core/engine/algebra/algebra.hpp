#pragma once

#include <cstddef>
#include <functional>

#include "core/engine/rules/tables/cayley_table.hpp"
#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/alphabet.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/word.hpp"
#include "word_adder.hpp"

class Algebra
{
private:
    Alphabet m_alphabet;
    Symbol m_zero;
    Symbol m_one;
    size_t m_word_size;

    CayleyTable m_addition_table;
    WordAdder m_adder;
    CayleyTable m_product_table;

public:
    Algebra(const TransitionManager &transition, const Alphabet &alphabet, size_t word_size);

    OperationResult add(const Word &x, const Word &y) const;
    OperationResult add(const OperationResult &x, const OperationResult &y) const;

    OperationResult multiply(const Word &x, const Word &y) const;
    OperationResult multiply(const OperationResult &x, const OperationResult &y) const;

    OperationResult subtract(const Word &x, const Word &y) const;
    OperationResult subtract(const OperationResult &x, const OperationResult &y) const;

    DivModResult divmod(const Word &x, const Word &y) const;
    DivModResult divmod(const OperationResult &x, const OperationResult &y) const;

    OperationResult div(const Word &x, const Word &y) const;
    OperationResult div(const OperationResult &x, const OperationResult &y) const;

    OperationResult mod(const Word &x, const Word &y) const;
    OperationResult mod(const OperationResult &x, const OperationResult &y) const;

    OperationResult divide(const Word &x, const Word &y) const;
    OperationResult divide(const OperationResult &x, const OperationResult &y) const;

private:
    using BinaryWordOp = OperationResult (Algebra::*)(const Word &, const Word &) const;
    using StepFn = std::function<OperationResult(const Word &acc, const Symbol &z, size_t idx)>;

    void _validate_same_size(const Word &x, const Word &y) const;
    void _validate_non_zero(const Word &y) const;

    OperationResult _apply_pairwise(const OperationResult &x, const OperationResult &y, BinaryWordOp op) const;
    OperationResult _sum_partials(const std::vector<OperationResult> &partials) const;

    OperationResult _multiply_digits(const Symbol &a, const Symbol &b, size_t shift, size_t word_size) const;
    OperationResult _multiply_by_symbol(const Word &x, const Symbol &s, size_t shift) const;

    OperationResult _solve_step(const Word &target, const Word &acc, size_t idx, const StepFn &step_fn) const;

    OperationResult _subtract_no_overflow(const Word &x, const Word &y) const;

    DivModResult _divmod_step(const Word &x, const Word &y) const;
};