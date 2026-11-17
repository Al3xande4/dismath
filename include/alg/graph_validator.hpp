#pragma once

#include "alg/alphabet.hpp"
#include "alg/symbol.hpp"
#include "alg/transition_graph.hpp"

namespace alg
{
class GraphValidator
{
public:
    static void validate(const TransitionGraph &graph, const Alphabet &alphabet, const Symbol &zero);
};
} // namespace alg