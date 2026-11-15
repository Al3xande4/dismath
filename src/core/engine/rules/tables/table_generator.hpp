#pragma once

#include <cstdlib>
#include <queue>
#include <utility>

#include "core/engine/rules/tables/cayley_table.hpp"
#include "core/engine/rules/tables/generation/generation_strategy.hpp"
#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/alphabet.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"

class TableGenerator
{
private:
    const TransitionManager &m_transition;
    const Alphabet &m_alphabet;
    Symbol m_zero;

public:
    TableGenerator(const TransitionManager &transition, const Alphabet &alphabet)
        : m_transition(transition), m_alphabet(alphabet), m_zero(transition.get_zero())
    {
    }

    CayleyTable generate(const IGenerationStrategy &strategy) const;

private:
    class GenerationSession
    {
        using State = std::pair<Symbol, Symbol>;

        const TableGenerator &generator;
        const IGenerationStrategy &strategy;
        CayleyTable table;

        std::queue<State> queue;
        std::set<Symbol> external;

    public:
        GenerationSession(const TableGenerator &generator, const IGenerationStrategy &strategy);

        CayleyTable generate();

    private:
        void _bfs();
        void _step(const OperationResult &prev, const Symbol &moving, const Symbol &fixed);
        void _record_symmetric(const Symbol &x, const Symbol &y, const OperationResult &value);
        void _set_initial_states();
        void _fill_unreachable_cells();
        void _set_axiom_values();
    };
};