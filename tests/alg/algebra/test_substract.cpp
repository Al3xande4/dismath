#include <alg/algebra.hpp>
#include <alg/alphabet.hpp>
#include <alg/errors.hpp>
#include <alg/result.hpp>
#include <alg/symbol.hpp>
#include <alg/term.hpp>
#include <alg/transition_graph.hpp>
#include <alg/word.hpp>

#include <utility>

#include <gtest/gtest.h>

using namespace alg;

TEST(AlgebraSubtractTest, Z4BorrowChainAndRingWrapAround)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 3);

    EXPECT_THROW(alg.subtract(Word(3, a), Word(2, a)), SizesMismatch);

    EXPECT_EQ(alg.subtract(Word({c, d, b}), Word({c, d, b})), (OperationResult{Word({a, a, a})}));
    EXPECT_EQ(alg.subtract(Word({a, a, b}), Word({b, a, a})), (OperationResult{Word({d, d, a})}));
    EXPECT_EQ(alg.subtract(Word({a, a, a}), Word({b, a, a})), (OperationResult{Word({d, d, d})}));
    EXPECT_EQ(alg.subtract(Word({b, a, a}), Word({d, d, d})), (OperationResult{Word({c, a, a})}));
}

TEST(AlgebraSubtractTest, DiamondGraphMultipleDifferences)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');
    Alphabet alphabet{a, b, c, d, e};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{e});
    graph.set(d, Term{e});
    graph.set(e, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 2);

    OperationResult expected_branch = {Word({c, a}), Word({d, a})};
    EXPECT_EQ(alg.subtract(Word({e, a}), Word({b, a})), expected_branch);
    EXPECT_EQ(alg.subtract(Word({a, b}), Word({c, a})), expected_branch);
    EXPECT_EQ(alg.subtract(Word({a, b}), Word({d, a})), expected_branch);
    EXPECT_EQ(alg.subtract(Word({a, b}), Word({e, a})), (OperationResult{Word({b, a})}));
}

TEST(AlgebraSubtractTest, ExternalElementsSelfSubtractionAndUnreachableTargets)
{
    Symbol a('a'), b('b'), e('e');
    Alphabet alphabet{a, b, e};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a});
    graph.set(e, Term{b});

    Algebra alg(std::move(graph), alphabet, a, 2);

    EXPECT_EQ(alg.subtract(Word({e, a}), Word({a, a})), (OperationResult{Word({e, a})}));
    EXPECT_EQ(alg.subtract(Word({e, a}), Word({e, a})), (OperationResult{Word({a, a})}));
    EXPECT_EQ(alg.subtract(Word({b, a}), Word({e, a})), (OperationResult{Word({b, a})}));
    EXPECT_TRUE(alg.subtract(Word({a, a}), Word({e, a})).empty());
}
