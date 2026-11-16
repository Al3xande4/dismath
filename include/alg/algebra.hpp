#pragma once

#include <cstddef>
#include <memory>

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

    Word make_word(const Symbol &);

    OperationResult add(const Word &x, const Word &y) const;
    OperationResult add(const OperationResult &x, const OperationResult &y) const;

    OperationResult multiply(const Word &x, const Word &y) const;
    OperationResult multiply(const OperationResult &x, const OperationResult &y) const;

    OperationResult subtract(const Word &x, const Word &y) const;
    OperationResult subtract(const OperationResult &x, const OperationResult &y) const;

    DivModResult divmod(const Word &x, const Word &y) const;
    DivModResult divmod(const OperationResult &x, const OperationResult &y) const;

    OperationResult div(const Word &x, const Word &y) const;
    OperationResult div(const OperationResult &x, const OperationResult &y) const;

    OperationResult mod(const Word &x, const Word &y) const;
    OperationResult mod(const OperationResult &x, const OperationResult &y) const;

    OperationResult divide(const Word &x, const Word &y) const;
    OperationResult divide(const OperationResult &x, const OperationResult &y) const;
};

} // namespace alg