#pragma once

#include "core/engine/rules/transition/transition_graph.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/term.hpp"

class TransitionManager
{
private:
    TransitionGraph m_transition_graph;
    Symbol m_zero;

public:
    TransitionManager(TransitionGraph transition_graph, Symbol zero)
        : m_transition_graph(std::move(transition_graph)), m_zero(std::move(zero))
    {
    }

    const Term &next(const Symbol &symbol) const;
    Term next(const Term &term) const;

    OperationResult next(const Word &word) const;

    OperationResult next(const OperationResult &operation_result) const;

    SymbolOperationResult next(const SymbolOperationResultItem &single_op_result) const;

    SymbolOperationResult next(const SymbolOperationResult &symbol_op_result) const;

    const Symbol &get_zero() const { return m_zero; }

private:
    OperationResult _increment_bit(const Word &word, size_t bit_idx) const;
};