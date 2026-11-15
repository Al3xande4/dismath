#include <gtest/gtest.h>

#include <cstddef>

#include "core/engine/algebra/word_adder.hpp"
#include "core/engine/rules/tables/cayley_table.hpp"
#include "core/engine/rules/tables/generation/addition_generation.hpp"
#include "core/engine/rules/tables/table_generator.hpp"
#include "core/engine/rules/transition/transition_graph.hpp"
#include "core/engine/rules/transition/transition_manager.hpp"
#include "core/entities/alphabet.hpp"
#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"
#include "core/entities/term.hpp"
#include "core/entities/word.hpp"

TEST(WordAdderHardcoreTest, DiamondGraphDivergesAtLowBitAndReconvergesViaCarry)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');
    Alphabet alphabet{a, b, c, d, e};
    size_t word_size = 3;

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{e});
    graph.set(d, Term{e});
    graph.set(e, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration add_strategy(tm, word_size);
    CayleyTable add_table = generator.generate(add_strategy);

    WordAdder adder(add_table, a);

    Word x({c, b, a});
    Word y({d, a, a});
    OperationResult expected = {Word({a, c, a}), Word({a, d, a})};
    EXPECT_EQ(adder.add(x, y), expected);

    Word u({e, c, b});
    Word v({b, b, a});
    OperationResult expected_reconverge = {Word({a, a, c}), Word({a, a, d})};
    EXPECT_EQ(adder.add(u, v), expected_reconverge);
}

TEST(WordAdderHardcoreTest, AsymmetricCyclesSecondOrderCarryLeapsOverMiddleDigit)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};
    size_t word_size = 4;

    TransitionGraph graph;
    graph.set(a, Term{b, c});
    graph.set(b, Term{a});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration add_strategy(tm, word_size);
    CayleyTable add_table = generator.generate(add_strategy);

    WordAdder adder(add_table, a);

    Word x({d, a, b, a});
    Word y({d, a, a, a});

    OperationResult res = adder.add(x, y);

    EXPECT_TRUE(res.find(Word({a, a, d, a})) != res.end());
    EXPECT_TRUE(res.find(Word({a, a, a, b})) != res.end());
    EXPECT_TRUE(res.find(Word({a, a, a, c})) != res.end());
    EXPECT_TRUE(res.find(Word({a, d, b, a})) != res.end());
}

TEST(WordAdderHardcoreTest, ShortcutGraphCombinatorialExplosionDeduplicatesCleanly)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};
    size_t word_size = 3;

    TransitionGraph graph;
    graph.set(a, Term{b, c});
    graph.set(b, Term{c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration add_strategy(tm, word_size);
    CayleyTable add_table = generator.generate(add_strategy);

    WordAdder adder(add_table, a);

    Word x({c, b, a});
    Word y({b, a, a});

    OperationResult res = adder.add(x, y);

    EXPECT_FALSE(res.empty());
    EXPECT_TRUE(res.find(Word({c, b, a})) != res.end());
    EXPECT_TRUE(res.find(Word({a, a, b})) != res.end());
    EXPECT_TRUE(res.find(Word({a, a, c})) != res.end());
}
