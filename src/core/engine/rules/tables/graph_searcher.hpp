#pragma once

#include <queue>
#include <set>

#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/alphabet.hpp"
#include "core/entities/symbol.hpp"

class GraphSearcher
{
public:
    static std::set<Symbol> find_reachable(const Symbol &from, const TransitionManager &transition)
    {
        std::set<Symbol> visited{from};
        std::queue<Symbol> queue;
        queue.push(from);

        while (!queue.empty())
        {
            auto &symbol = queue.front();
            queue.pop();

            for (const auto &next : transition.next(symbol))
                if (!visited.contains(next))
                {
                    visited.emplace(next);
                    queue.emplace(next);
                }
        }

        return visited;
    }

    static std::set<Symbol> find_external(const Symbol &from, const Alphabet &all, const TransitionManager &transition)
    {
        auto reachable = find_reachable(from, transition);

        std::set<Symbol> external;

        for (const auto &symbol : all)
            if (!reachable.contains(symbol))
                external.emplace(symbol);

        return external;
    }
};