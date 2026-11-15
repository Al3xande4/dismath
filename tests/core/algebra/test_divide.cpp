#include <gtest/gtest.h>

#include "core/api/errors.hpp"
#include "core/engine/algebra/algebra.hpp"
#include "core/engine/rules/transition/transition_graph.hpp"
#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/alphabet.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/term.hpp"
#include "core/entities/word.hpp"

TEST(AlgebraDivideTest, Z4InvertibleElementsAndZeroDivisors)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);
    Algebra alg(tm, alphabet, 3);

    EXPECT_THROW(alg.divide(Word(3, a), Word(2, a)), SizesMismatch);
    EXPECT_THROW(alg.divide(Word({b, a, a}), Word({a, a, a})), DivisionByZero);

    EXPECT_EQ(alg.divide(Word({b, c, a}), Word({d, a, a})), (OperationResult{Word({d, a, a})}));
    EXPECT_EQ(alg.divide(Word({b, a, a}), Word({d, a, a})), (OperationResult{Word({d, c, c})}));
    EXPECT_EQ(alg.divide(Word({b, a, a}), Word({d, d, d})), (OperationResult{Word({d, d, d})}));

    EXPECT_TRUE(alg.divide(Word({b, a, a}), Word({c, a, a})).empty());

    OperationResult expected_roots = {Word({b, a, a}), Word({b, a, c})};
    EXPECT_EQ(alg.divide(Word({c, a, a}), Word({c, a, a})), expected_roots);
}

TEST(AlgebraDivideTest, DiamondGraphBranchingExactDivision)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');
    Alphabet alphabet{a, b, c, d, e};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{e});
    graph.set(d, Term{e});
    graph.set(e, Term{a});

    TransitionManager tm(std::move(graph), a);
    Algebra alg(tm, alphabet, 2);

    EXPECT_EQ(alg.divide(Word({e, a}), Word({b, a})), (OperationResult{Word({e, a})}));

    OperationResult expected_ab_div_c = {
        Word({c, a}), Word({d, a}), Word({c, c}), Word({d, c}), Word({c, d}), Word({d, d})
    };
    EXPECT_EQ(alg.divide(Word({a, b}), Word({c, a})), expected_ab_div_c);
}
