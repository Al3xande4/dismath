#include <alg/errors.hpp>
#include <alg/graph_validator.hpp>

#include <queue>
#include <set>
#include <vector>

namespace alg
{

namespace
{

void validate_alphabet_and_zero(const Alphabet &alphabet, const Symbol &zero)
{
    if (alphabet.empty())
        throw EmptyAlphabet();

    if (!alphabet.contains(zero))
        throw ZeroNotInAlphabet(zero);
}

void validate_structure(const TransitionGraph &graph, const Alphabet &alphabet)
{
    for (const auto &symbol : alphabet)
    {
        if (!graph.contains(symbol))
            throw MissingTransition(symbol);

        const auto &term = graph.get(symbol);
        if (term.empty())
            throw EmptyTransition(symbol);

        for (const auto &target : term)
        {
            if (!alphabet.contains(target))
                throw TargetNotInAlphabet(symbol, target);
        }
    }
}

void validate_unit(const TransitionGraph &graph, const Symbol &zero)
{
    const auto &zero_term = graph.get(zero);

    if (zero_term.size() != 1)
        throw AmbiguousOne(zero);

    if (*zero_term.begin() == zero)
        throw DegenerateOne(zero);
}

void validate_return_to_zero(const TransitionGraph &graph, const Alphabet &alphabet, const Symbol &zero)
{
    std::map<Symbol, std::vector<Symbol>> reverse_edges;
    for (const auto &symbol : alphabet)
    {
        if (graph.contains(symbol))
        {
            for (const auto &target : graph.get(symbol))
                reverse_edges[target].push_back(symbol);
        }
    }

    std::set<Symbol> visited{zero};
    std::queue<Symbol> queue;
    queue.push(zero);

    while (!queue.empty())
    {
        Symbol current = queue.front();
        queue.pop();

        for (const auto &prev : reverse_edges[current])
        {
            if (visited.insert(prev).second)
                queue.push(prev);
        }
    }

    if (visited.size() != alphabet.size())
    {
        std::vector<Symbol> trapped;
        for (const auto &sym : alphabet)
        {
            if (!visited.contains(sym))
                trapped.push_back(sym);
        }
        throw TrappedNodes(std::move(trapped));
    }
}

} // namespace

void GraphValidator::validate(const TransitionGraph &graph, const Alphabet &alphabet, const Symbol &zero)
{
    validate_alphabet_and_zero(alphabet, zero);
    validate_structure(graph, alphabet);
    validate_unit(graph, zero);
    validate_return_to_zero(graph, alphabet, zero);
}

} // namespace alg