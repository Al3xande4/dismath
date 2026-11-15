#pragma once

#include <cstddef>

#include "core/api/errors.hpp"
#include "core/engine/algebra/word_adder.hpp"
#include "core/engine/rules/tables/generation/generation_strategy.hpp"
#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/word.hpp"

class ProductGeneration final : public IGenerationStrategy
{
private:
    const WordAdder &m_addition;
    Symbol m_zero;
    Symbol m_one;
    size_t m_word_size;

public:
    ProductGeneration(const ProductGeneration &) = default;
    ProductGeneration(ProductGeneration &&) = default;
    ProductGeneration &operator=(const ProductGeneration &) = delete;
    ProductGeneration &operator=(ProductGeneration &&) = delete;

    ProductGeneration(const WordAdder &addition, Symbol zero, Symbol one, size_t word_size)
        : m_addition(addition), m_zero(std::move(zero)), m_one(one), m_word_size(word_size)
    {
    }

    bool is_axiom(const Symbol &x, const Symbol &y) const override
    {
        return x == m_zero || y == m_zero || x == m_one || y == m_one;
    }

    OperationResult axiom_value(const Symbol &x, const Symbol &y) const override
    {
        if (x == m_zero || y == m_zero)
            return {Word(m_word_size, m_zero)};

        if (x == m_one)
            return {Word(m_word_size, m_zero, {y})};

        if (y == m_one)
            return {Word(m_word_size, m_zero, {x})};

        throw AxiomError("ProductGeneration.axiom_value: neither x nor y fall into axiom cases");
    }

    OperationResult advance(const OperationResult &prev, const Symbol &fixed) const override
    {
        return m_addition.add(prev, {Word(m_word_size, m_zero, {fixed})});
    }
};