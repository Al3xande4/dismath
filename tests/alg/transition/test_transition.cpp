#include <alg/result.hpp>
#include <alg/symbol.hpp>
#include <alg/term.hpp>
#include <alg/transition_graph.hpp>
#include <alg/word.hpp>

#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include "transition/transition_manager.hpp"

using namespace alg;

TEST(TransitionManagerTest, NextSymbolDeterministicAndBranching)
{
    Symbol a('a'), b('b'), c('c'), unknown('z');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a, c});

    TransitionManager tm(std::move(graph), a);

    EXPECT_EQ(tm.next(a), (Term{b}));
    EXPECT_EQ(tm.next(b), (Term{a, c}));
    EXPECT_THROW(tm.next(unknown), std::out_of_range);
}

TEST(TransitionManagerTest, NextTermMergesAndDeduplicates)
{
    Symbol a('a'), b('b'), c('c'), d('d');

    TransitionGraph graph;
    graph.set(a, Term{b, c});
    graph.set(b, Term{c, d});
    graph.set(c, Term{a});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);

    EXPECT_EQ(tm.next(Term{}), Term{});
    EXPECT_EQ(tm.next(Term{a, b}), (Term{b, c, d}));
}

TEST(TransitionManagerTest, NextWordSimpleAndCascadeCarry)
{
    Symbol a('a'), b('b'), c('c');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);

    EXPECT_EQ(tm.next(Word({a, a, a})), (OperationResult{Word({b, a, a})}));
    EXPECT_EQ(tm.next(Word({c, a, a})), (OperationResult{Word({a, b, a})}));
    EXPECT_EQ(tm.next(Word({c, c, a})), (OperationResult{Word({a, a, b})}));
    EXPECT_EQ(tm.next(Word({c, c, c})), (OperationResult{Word({a, a, a})}));
}

TEST(TransitionManagerTest, NextWordMixedBranchingWithAndWithoutCarry)
{
    Symbol a('a'), b('b'), c('c');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a, c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);

    OperationResult expected = {Word({c, a, a}), Word({a, b, a})};
    EXPECT_EQ(tm.next(Word({b, a, a})), expected);
}

TEST(TransitionManagerTest, NextWordBranchingInsideCascadeCarry)
{
    Symbol a('a'), b('b'), c('c'), d('d');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{a});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);

    OperationResult expected = {Word({a, c, a}), Word({a, d, a})};
    EXPECT_EQ(tm.next(Word({c, b, a})), expected);
}

TEST(TransitionManagerTest, SelfLoopOnZeroTerminatesAtWordBoundary)
{
    Symbol a('a'), b('b');

    TransitionGraph graph;
    graph.set(a, Term{a, b});
    graph.set(b, Term{a});

    TransitionManager tm(std::move(graph), a);

    OperationResult expected = {Word({b, a}), Word({a, b}), Word({a, a})};
    EXPECT_EQ(tm.next(Word({a, a})), expected);
}

TEST(TransitionManagerTest, DiamondGraphConvergesAndDeduplicatesCarry)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{e});
    graph.set(d, Term{e});
    graph.set(e, Term{a});

    TransitionManager tm(std::move(graph), a);

    EXPECT_EQ(tm.next(Term{c, d}), (Term{e}));

    OperationResult split_carry = tm.next(Word({e, b, a}));
    OperationResult expected_split = {Word({a, c, a}), Word({a, d, a})};
    EXPECT_EQ(split_carry, expected_split);

    OperationResult deep_split = tm.next(Word({e, e, b}));
    OperationResult expected_deep = {Word({a, a, c}), Word({a, a, d})};
    EXPECT_EQ(deep_split, expected_deep);
}

TEST(TransitionManagerTest, ConvergingToZeroSimultaneouslyDeduplicatesCascade)
{
    Symbol a('a'), b('b'), c('c'), d('d');

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{a});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);

    EXPECT_EQ(tm.next(Word({c, a, a})), (OperationResult{Word({a, b, a})}));
    EXPECT_EQ(tm.next(Word({d, a, a})), (OperationResult{Word({a, b, a})}));
}

TEST(TransitionManagerTest, ShortcutGraphMultiLevelCascade)
{
    Symbol a('a'), b('b'), c('c');

    TransitionGraph graph;
    graph.set(a, Term{b, c});
    graph.set(b, Term{c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);

    OperationResult res1 = tm.next(Word({c, a, a}));
    OperationResult expected1 = {Word({a, b, a}), Word({a, c, a})};
    EXPECT_EQ(res1, expected1);

    OperationResult res2 = tm.next(Word({c, c, a}));
    OperationResult expected2 = {Word({a, a, b}), Word({a, a, c})};
    EXPECT_EQ(res2, expected2);

    OperationResult res_overflow = tm.next(Word({c, c, c}));
    EXPECT_EQ(res_overflow, (OperationResult{Word({a, a, a})}));
}
