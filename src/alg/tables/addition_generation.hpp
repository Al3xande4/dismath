#pragma once

#include <cstddef>

#include "alg/result.hpp"
#include "generation_strategy.hpp"
#include "transition/transition_manager.hpp"

namespace alg
{

class AdditionGeneration final : public IGenerationStrategy
{
private:
    const TransitionManager &m_transition;
    Symbol m_zero;
    size_t m_word_size;

public:
    AdditionGeneration(const AdditionGeneration &) = default;
    AdditionGeneration(AdditionGeneration &&) = default;
    AdditionGeneration &operator=(const AdditionGeneration &) = delete;
    AdditionGeneration &operator=(AdditionGeneration &&) = delete;

    AdditionGeneration(const TransitionManager &transition, size_t word_size)
        : m_transition(transition), m_zero(transition.get_zero()), m_word_size(word_size)
    {
    }

    bool is_axiom(const Symbol &x, const Symbol &y) const override { return x == m_zero || y == m_zero; }

    OperationResult axiom_value(const Symbol &x, const Symbol &y) const override
    {
        if (x == m_zero)
            return {Word(m_word_size, m_zero, {y})};

        if (y == m_zero)
            return {Word(m_word_size, m_zero, {x})};

        throw AxiomError("AdditionGeneration.axiom_value: neither x nor y fall into axiom cases");
    }

    OperationResult advance(const OperationResult &prev, const Symbol &) const override
    {
        return m_transition.next(prev);
    }
};

} // namespace alg
