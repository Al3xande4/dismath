#include "table_generator.hpp"

#include <queue>

#include "graph_searcher.hpp"

namespace alg
{

TableGenerator::GenerationSession::GenerationSession(
    const TableGenerator &generator,
    const IGenerationStrategy &strategy
)
    : generator(generator), strategy(strategy),
      external(GraphSearcher::find_external(generator.m_zero, generator.m_alphabet, generator.m_transition))
{
}

void TableGenerator::GenerationSession::_set_initial_states()
{
    _record_symmetric(generator.m_zero, generator.m_zero, strategy.axiom_value(generator.m_zero, generator.m_zero));
    for (const auto &ext : external)
        _record_symmetric(ext, generator.m_zero, strategy.axiom_value(ext, generator.m_zero));
}

void TableGenerator::GenerationSession::_record_symmetric(
    const Symbol &x,
    const Symbol &y,
    const OperationResult &value
)
{
    bool changed_xy = table.merge(x, y, value);
    bool changed_yx = table.merge(y, x, value);

    if (changed_xy || changed_yx)
        queue.emplace(x, y);
}

void TableGenerator::GenerationSession::_fill_unreachable_cells()
{
    for (const auto &x : generator.m_alphabet)
        for (const auto &y : generator.m_alphabet)
            if (!table.contains(x, y))
                table.set(x, y, {});
}

void TableGenerator::GenerationSession::_set_axiom_values()
{
    for (const auto &x : generator.m_alphabet)
        for (const auto &y : generator.m_alphabet)
            if (strategy.is_axiom(x, y))
                table.set(x, y, strategy.axiom_value(x, y));
}

void TableGenerator::GenerationSession::_step(const OperationResult &prev, const Symbol &moving, const Symbol &fixed)
{
    if (external.contains(moving))
        return;

    auto next_res = strategy.advance(prev, fixed);

    for (const auto &next : generator.m_transition.next(moving))
        if (next != generator.m_zero)
            _record_symmetric(next, fixed, next_res);
}

void TableGenerator::GenerationSession::_bfs()
{
    while (!queue.empty())
    {
        auto [x, y] = queue.front();
        queue.pop();

        const OperationResult prev = table.get(x, y);

        _step(prev, x, y);
        if (x != y)
            _step(prev, y, x);
    }
}

CayleyTable TableGenerator::GenerationSession::generate()
{
    _set_initial_states();
    _bfs();
    _fill_unreachable_cells();
    _set_axiom_values();

    return table;
}

CayleyTable TableGenerator::generate(const IGenerationStrategy &strategy) const
{
    GenerationSession session(*this, strategy);

    return session.generate();
}

} // namespace alg
