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

TEST(AlgebraMultiplyTest, Z4AxiomsCarriesAndRingTruncation)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 3);

    EXPECT_THROW(alg.multiply(Word(3, a), Word(2, a)), SizesMismatch);

    EXPECT_EQ(alg.multiply(Word({d, c, b}), Word({a, a, a})), (OperationResult{Word({a, a, a})}));
    EXPECT_EQ(alg.multiply(Word({d, c, b}), Word({b, a, a})), (OperationResult{Word({d, c, b})}));
    EXPECT_EQ(alg.multiply(Word({d, a, a}), Word({d, a, a})), (OperationResult{Word({b, c, a})}));
    EXPECT_EQ(alg.multiply(Word({d, d, a}), Word({c, a, a})), (OperationResult{Word({c, d, b})}));
    EXPECT_EQ(alg.multiply(Word({d, d, d}), Word({d, d, d})), (OperationResult{Word({b, a, a})}));
    EXPECT_EQ(alg.multiply(Word({a, a, c}), Word({a, c, a})), (OperationResult{Word({a, a, a})}));
}

TEST(AlgebraMultiplyTest, DiamondGraphBranchingProducts)
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

    EXPECT_EQ(alg.multiply(Word({c, a}), Word({c, a})), (OperationResult{Word({a, b})}));
    EXPECT_EQ(alg.multiply(Word({c, a}), Word({d, a})), (OperationResult{Word({a, b})}));

    OperationResult expected_ce = {Word({c, b}), Word({d, b})};
    EXPECT_EQ(alg.multiply(Word({c, a}), Word({e, a})), expected_ce);

    OperationResult expected_ee = {Word({b, c}), Word({b, d})};
    EXPECT_EQ(alg.multiply(Word({e, a}), Word({e, a})), expected_ee);
}

TEST(AlgebraMultiplyTest, AsymmetricCyclesSecondOrderCarryInProduct)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a, c});
    graph.set(c, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 3);

    OperationResult prod = alg.multiply(Word({c, a, a}), Word({c, a, a}));
    EXPECT_TRUE(prod.contains(Word({a, a, b})));
    EXPECT_TRUE(prod.contains(Word({a, c, a})));
    EXPECT_TRUE(prod.contains(Word({b, b, a})));
    EXPECT_TRUE(prod.contains(Word({c, b, a})));
}

TEST(AlgebraMultiplyTest, ExternalElementAxiomsAndAnnihilation)
{
    Symbol a('a'), b('b'), c('c'), e('e');
    Alphabet alphabet{a, b, c, e};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{a});
    graph.set(e, Term{b});

    Algebra alg(std::move(graph), alphabet, a, 2);

    EXPECT_EQ(alg.multiply(Word({e, a}), Word({a, a})), (OperationResult{Word({a, a})}));
    EXPECT_EQ(alg.multiply(Word({e, a}), Word({b, a})), (OperationResult{Word({e, a})}));
    EXPECT_EQ(alg.multiply(Word({e, a}), Word({a, b})), (OperationResult{Word({a, e})}));
    EXPECT_TRUE(alg.multiply(Word({e, a}), Word({c, a})).empty());
    EXPECT_TRUE(alg.multiply(Word({e, a}), Word({e, a})).empty());
}
