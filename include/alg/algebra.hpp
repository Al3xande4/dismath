#pragma once

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <variant>

#include "alg/alphabet.hpp"
#include "alg/result.hpp"
#include "alg/symbol.hpp"
#include "alg/transition_graph.hpp"
#include "alg/word.hpp"

namespace alg
{

class Algebra
{
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

public:
    Algebra(TransitionGraph transition_graph, Alphabet alphabet, Symbol zero, size_t word_size);

    ~Algebra();
    Algebra(Algebra &&) noexcept;
    Algebra &operator=(Algebra &&) noexcept;

    Word make_word(std::initializer_list<Symbol> symbols) const;
    Word parse_word(std::string_view text) const;
    Word zero_word() const;
    Word one_word() const;

    using Operand = std::variant<Word, OperationResult>;

    OperationResult add(const Operand &, const Operand &) const;

    OperationResult multiply(const Operand &, const Operand &) const;

    OperationResult subtract(const Operand &, const Operand &) const;

    DivModResult divmod(const Operand &, const Operand &) const;

    OperationResult div(const Operand &, const Operand &) const;

    OperationResult mod(const Operand &, const Operand &) const;

    OperationResult divide(const Operand &, const Operand &) const;
};

} // namespace alg