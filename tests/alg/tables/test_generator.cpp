#include <alg/alphabet.hpp>
#include <alg/result.hpp>
#include <alg/symbol.hpp>
#include <alg/term.hpp>
#include <alg/transition_graph.hpp>
#include <alg/word.hpp>

#include <set>
#include <utility>

#include <gtest/gtest.h>

#include "algebra/word_adder.hpp"
#include "tables/addition_generation.hpp"
#include "tables/cayley_table.hpp"
#include "tables/graph_searcher.hpp"
#include "tables/product_generation.hpp"
#include "tables/table_generator.hpp"
#include "transition/transition_manager.hpp"

using namespace alg;

TEST(GraphSearcherTest, FindsExternalElementsCorrectly)
{
    Symbol a('a'), b('b'), e1('x'), e2('y');
    Alphabet alphabet{a, b, e1, e2};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a});
    graph.set(e1, Term{e2});
    graph.set(e2, Term{b});

    TransitionManager tm(std::move(graph), a);

    std::set<Symbol> externals = GraphSearcher::find_external(a, alphabet, tm);
    std::set<Symbol> expected = {e1, e2};

    EXPECT_EQ(externals, expected);
}

TEST(TableGeneratorAdditionTest, CyclicGroupZ3FullTable)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};
    size_t word_size = 2;

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration strategy(tm, word_size);

    CayleyTable table = generator.generate(strategy);

    EXPECT_EQ(table.get(a, a), (OperationResult{Word({a, a})}));
    EXPECT_EQ(table.get(a, b), (OperationResult{Word({b, a})}));
    EXPECT_EQ(table.get(c, a), (OperationResult{Word({c, a})}));

    EXPECT_EQ(table.get(b, b), (OperationResult{Word({c, a})}));
    EXPECT_EQ(table.get(b, c), (OperationResult{Word({a, b})}));
    EXPECT_EQ(table.get(c, c), (OperationResult{Word({b, b})}));
}

TEST(TableGeneratorAdditionTest, ExternalChainAndUnreachableCells)
{
    Symbol a('a'), b('b'), e1('x'), e2('y');
    Alphabet alphabet{a, b, e1, e2};
    size_t word_size = 2;

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a});
    graph.set(e1, Term{e2});
    graph.set(e2, Term{b});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration strategy(tm, word_size);

    CayleyTable table = generator.generate(strategy);

    EXPECT_EQ(table.get(e1, a), (OperationResult{Word({e1, a})}));
    EXPECT_EQ(table.get(a, e2), (OperationResult{Word({e2, a})}));

    EXPECT_EQ(table.get(e1, b), (OperationResult{Word({e2, a})}));
    EXPECT_EQ(table.get(b, e1), (OperationResult{Word({e2, a})}));
    EXPECT_EQ(table.get(e2, b), (OperationResult{Word({b, a})}));

    EXPECT_TRUE(table.get(e1, e1).empty());
    EXPECT_TRUE(table.get(e2, e2).empty());
    EXPECT_TRUE(table.get(e1, e2).empty());
    EXPECT_TRUE(table.get(e2, e1).empty());
}

TEST(TableGeneratorAdditionTest, BranchingGraphProtectsAxioms)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};
    size_t word_size = 2;

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c, d});
    graph.set(c, Term{a});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration strategy(tm, word_size);

    CayleyTable table = generator.generate(strategy);

    EXPECT_EQ(table.get(c, a), (OperationResult{Word({c, a})}));
    EXPECT_EQ(table.get(d, a), (OperationResult{Word({d, a})}));

    OperationResult expected_bb = {Word({c, a}), Word({d, a})};
    EXPECT_EQ(table.get(b, b), expected_bb);
}

TEST(TableGeneratorProductTest, CyclicGroupZ4MultiplicationAndExternal)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');
    Alphabet alphabet{a, b, c, d, e};
    size_t word_size = 2;

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{c});
    graph.set(c, Term{d});
    graph.set(d, Term{a});
    graph.set(e, Term{c});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);

    AdditionGeneration add_strategy(tm, word_size);
    CayleyTable add_table = generator.generate(add_strategy);

    WordAdder adder(add_table, a);
    ProductGeneration prod_strategy(adder, a, b, word_size);
    CayleyTable prod_table = generator.generate(prod_strategy);

    EXPECT_EQ(prod_table.get(e, a), (OperationResult{Word({a, a})}));
    EXPECT_EQ(prod_table.get(a, d), (OperationResult{Word({a, a})}));
    EXPECT_EQ(prod_table.get(e, b), (OperationResult{Word({e, a})}));
    EXPECT_EQ(prod_table.get(b, d), (OperationResult{Word({d, a})}));

    EXPECT_EQ(prod_table.get(c, c), (OperationResult{Word({a, b})}));
    EXPECT_EQ(prod_table.get(c, d), (OperationResult{Word({c, b})}));
    EXPECT_EQ(prod_table.get(d, d), (OperationResult{Word({b, c})}));

    EXPECT_TRUE(prod_table.get(e, c).empty());
    EXPECT_TRUE(prod_table.get(e, d).empty());
    EXPECT_TRUE(prod_table.get(e, e).empty());
}

TEST(TableGeneratorEdgeCasesTest, DiamondGraphDivergenceAndConvergence)
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

    CayleyTable table = generator.generate(add_strategy);

    EXPECT_EQ(table.get(c, a), (OperationResult{Word({c, a, a})}));
    EXPECT_EQ(table.get(d, a), (OperationResult{Word({d, a, a})}));
    EXPECT_EQ(table.get(e, a), (OperationResult{Word({e, a, a})}));

    OperationResult expected_bb = {Word({c, a, a}), Word({d, a, a})};
    EXPECT_EQ(table.get(b, b), expected_bb);

    EXPECT_EQ(table.get(c, b), (OperationResult{Word({e, a, a})}));
    EXPECT_EQ(table.get(d, b), (OperationResult{Word({e, a, a})}));

    EXPECT_EQ(table.get(c, c), (OperationResult{Word({a, b, a})}));
    EXPECT_EQ(table.get(c, d), (OperationResult{Word({a, b, a})}));
    EXPECT_EQ(table.get(d, d), (OperationResult{Word({a, b, a})}));
    EXPECT_EQ(table.get(e, b), (OperationResult{Word({a, b, a})}));

    EXPECT_EQ(table.get(e, c), (OperationResult{Word({b, b, a})}));
    EXPECT_EQ(table.get(e, d), (OperationResult{Word({b, b, a})}));

    OperationResult expected_ee = {Word({c, b, a}), Word({d, b, a})};
    EXPECT_EQ(table.get(e, e), expected_ee);
}

TEST(TableGeneratorEdgeCasesTest, ShortcutGraphProducesSevenAndNineVariants)
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

    CayleyTable table = generator.generate(add_strategy);

    EXPECT_EQ(table.get(b, a), (OperationResult{Word({b, a, a})}));
    EXPECT_EQ(table.get(c, a), (OperationResult{Word({c, a, a})}));

    OperationResult expected_bb = {Word({c, a, a}), Word({a, b, a}), Word({a, c, a})};
    EXPECT_EQ(table.get(b, b), expected_bb);

    OperationResult expected_cb = {
        Word({c, a, a}),
        Word({a, b, a}),
        Word({a, c, a}),
        Word({b, b, a}),
        Word({c, b, a}),
        Word({b, c, a}),
        Word({c, c, a})
    };
    EXPECT_EQ(table.get(c, b).size(), 7U);
    EXPECT_EQ(table.get(c, b), expected_cb);
    EXPECT_EQ(table.get(b, c), expected_cb);

    OperationResult expected_cc = {
        Word({c, a, a}),
        Word({a, b, a}),
        Word({a, c, a}),
        Word({b, b, a}),
        Word({c, b, a}),
        Word({b, c, a}),
        Word({c, c, a}),
        Word({a, a, b}),
        Word({a, a, c})
    };
    EXPECT_EQ(table.get(c, c).size(), 9U);
    EXPECT_EQ(table.get(c, c), expected_cc);
}

TEST(TableGeneratorEdgeCasesTest, AsymmetricFigureEightGeneratesSecondOrderCarry)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};
    size_t word_size = 3;

    TransitionGraph graph;
    graph.set(a, Term{b, c});
    graph.set(b, Term{a});
    graph.set(c, Term{d});
    graph.set(d, Term{a});

    TransitionManager tm(std::move(graph), a);
    TableGenerator generator(tm, alphabet);
    AdditionGeneration add_strategy(tm, word_size);

    CayleyTable table = generator.generate(add_strategy);

    OperationResult expected_bb = {Word({d, a, a}), Word({a, b, a}), Word({a, c, a})};
    EXPECT_EQ(table.get(b, b), expected_bb);
    EXPECT_EQ(table.get(b, c), expected_bb);
    EXPECT_EQ(table.get(c, c), expected_bb);

    OperationResult expected_db = {
        Word({a, b, a}), Word({a, c, a}), Word({b, b, a}), Word({c, b, a}), Word({b, c, a}), Word({c, c, a})
    };
    EXPECT_EQ(table.get(d, b), expected_db);
    EXPECT_EQ(table.get(d, c), expected_db);

    OperationResult expected_dd = {
        Word({b, b, a}),
        Word({c, b, a}),
        Word({b, c, a}),
        Word({c, c, a}),
        Word({d, b, a}),
        Word({d, c, a}),
        Word({a, d, a}),
        Word({a, a, b}),
        Word({a, a, c})
    };
    EXPECT_EQ(table.get(d, d), expected_dd);
}