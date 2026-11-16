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

TEST(AlgebraDivModTest, Z4StandardEuclideanDivisionCases)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 3);

    EXPECT_THROW(alg.divmod(Word(3, a), Word(2, a)), SizesMismatch);
    EXPECT_THROW(alg.divmod(Word({b, a, a}), Word({a, a, a})), DivisionByZero);
    EXPECT_THROW(alg.div(Word({b, a, a}), Word({a, a, a})), DivisionByZero);
    EXPECT_THROW(alg.mod(Word({b, a, a}), Word({a, a, a})), DivisionByZero);

    EXPECT_EQ(alg.divmod(Word({a, a, a}), Word({d, a, a})), (DivModResult{{Word({a, a, a}), Word({a, a, a})}}));

    EXPECT_EQ(alg.divmod(Word({c, a, a}), Word({d, a, a})), (DivModResult{{Word({a, a, a}), Word({c, a, a})}}));

    EXPECT_EQ(alg.divmod(Word({d, a, a}), Word({d, a, a})), (DivModResult{{Word({b, a, a}), Word({a, a, a})}}));

    EXPECT_EQ(alg.divmod(Word({d, c, a}), Word({d, a, a})), (DivModResult{{Word({d, a, a}), Word({c, a, a})}}));

    EXPECT_EQ(alg.div(Word({d, c, a}), Word({d, a, a})), (OperationResult{Word({d, a, a})}));
    EXPECT_EQ(alg.mod(Word({d, c, a}), Word({d, a, a})), (OperationResult{Word({c, a, a})}));
}

TEST(AlgebraDivModTest, DiamondGraphProducesBranchingQuotientsAndRemainders)
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

    DivModResult dm_cb = alg.divmod(Word({c, a}), Word({b, a}));
    DivModResult expected_cb = {{Word({c, a}), Word({a, a})}, {Word({d, a}), Word({a, a})}};
    EXPECT_EQ(dm_cb, expected_cb);

    DivModResult dm_ec = alg.divmod(Word({e, a}), Word({c, a}));
    DivModResult expected_ec = {{Word({b, a}), Word({b, a})}};
    EXPECT_EQ(dm_ec, expected_ec);

    DivModResult dm_eb = alg.divmod(Word({e, a}), Word({b, a}));
    DivModResult expected_eb = {{Word({e, a}), Word({a, a})}};
    EXPECT_EQ(dm_eb, expected_eb);
}

TEST(AlgebraDivModTest, AsymmetricCyclesSecondOrderCarryTrapPreventsFalseBorrow)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a, c});
    graph.set(c, Term{a});

    Algebra alg(std::move(graph), alphabet, a, 3);

    DivModResult dm_less = alg.divmod(Word({b, a, a}), Word({a, b, a}));
    DivModResult expected_less = {{Word({a, a, a}), Word({b, a, a})}};
    EXPECT_EQ(dm_less, expected_less);

    DivModResult dm_cb = alg.divmod(Word({c, a, a}), Word({b, a, a}));
    EXPECT_TRUE(dm_cb.contains({Word({c, a, a}), Word({a, a, a})}));
}
