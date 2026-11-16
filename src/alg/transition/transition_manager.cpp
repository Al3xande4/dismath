#include "transition_manager.hpp"

namespace alg
{

const Term &TransitionManager::next(const Symbol &symbol) const
{
    return m_transition_graph.get(symbol);
}

Term TransitionManager::next(const Term &term) const
{
    Term result;

    for (const auto &symbol : term)
        result.merge(next(symbol));

    return result;
}

OperationResult TransitionManager::_increment_bit(const Word &word, size_t bit_idx) const
{
    if (bit_idx >= word.size())
        return {word};

    OperationResult result;

    Term next_bits = next(word[bit_idx]);
    for (const auto &next_bit : next_bits)
    {
        Word next_word(word);
        next_word[bit_idx] = next_bit;

        if (next_bit == m_zero)
            result.merge(_increment_bit(next_word, bit_idx + 1));
        else
            result.emplace(std::move(next_word));
    }

    return result;
}

OperationResult TransitionManager::next(const Word &word) const
{
    return _increment_bit(word, 0);
}

OperationResult TransitionManager::next(const OperationResult &operation_result) const
{
    OperationResult result;
    for (const auto &word : operation_result)
        result.merge(next(word));

    return result;
}

SymbolOperationResult TransitionManager::next(const SymbolOperationResultItem &single_op_result) const
{
    SymbolOperationResult result;

    const auto &next_symbols = next(single_op_result.value);

    for (const auto &next_symbol : next_symbols)
        if (next_symbol == m_zero)
            for (auto &&next_carry : next(single_op_result.carry))
                result.emplace(next_symbol, next_carry);
        else
            result.emplace(next_symbol, single_op_result.carry);

    return result;
}

SymbolOperationResult TransitionManager::next(const SymbolOperationResult &symbol_op_result) const
{
    SymbolOperationResult result;
    for (const auto &single_res : symbol_op_result)
        result.merge(next(single_res));

    return result;
}

} // namespace alg