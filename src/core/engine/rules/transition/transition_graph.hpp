#pragma once

#include <map>

#include "core/entities/symbol.hpp"
#include "core/entities/term.hpp"

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

    const Term &get(const Symbol &symbol) const { return m_graph.at(symbol); }
};