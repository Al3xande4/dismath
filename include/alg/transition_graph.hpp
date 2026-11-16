#pragma once

#include <alg/symbol.hpp>
#include <alg/term.hpp>

#include <initializer_list>
#include <map>
#include <utility>

namespace alg
{

class TransitionGraph
{
private:
    std::map<Symbol, Term> m_graph;

public:
    TransitionGraph() = default;

    TransitionGraph(std::initializer_list<std::pair<const Symbol, Term>> rules) : m_graph(rules) {}

    explicit TransitionGraph(std::map<Symbol, Term> graph) : m_graph(std::move(graph)) {}

    bool contains(const Symbol &symbol) const { return m_graph.contains(symbol); }

    void set(const Symbol &symbol, const Term &term) { m_graph[symbol] = term; }

    const std::map<Symbol, Term> &rules() const noexcept { return m_graph; };

    const Term &get(const Symbol &symbol) const { return m_graph.at(symbol); }
};

} // namespace alg
