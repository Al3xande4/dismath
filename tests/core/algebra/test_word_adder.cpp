#include <gtest/gtest.h>

#include <cstddef>
#include <initializer_list>

#include "core/api/errors.hpp"
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

class WordAdderZ4Test : public ::testing::Test
{
protected:
    Symbol a{'a'};
    Symbol b{'b'};
    Symbol c{'c'};
    Symbol d{'d'};
    size_t table_word_size = 3;
    Alphabet alphabet{a, b, c, d};

    TransitionManager make_tm() const
    {
        TransitionGraph graph;
        graph.set(a, Term{b});
        graph.set(b, Term{c});
        graph.set(c, Term{d});
        graph.set(d, Term{a});
        return TransitionManager(std::move(graph), a);
    }

    CayleyTable make_table(const TransitionManager &tm) const
    {
        return TableGenerator(tm, alphabet).generate(AdditionGeneration(tm, table_word_size));
    }
};

TEST_F(WordAdderZ4Test, ThrowsOnSizeMismatch)
{
    TransitionManager tm = make_tm();
    CayleyTable table = make_table(tm);
    WordAdder adder(table, a);

    EXPECT_THROW(adder.add(Word(3, a), Word(4, a)), SizesMismatch);
    EXPECT_THROW(adder.add(Word(5, a), Word(2, a)), SizesMismatch);
}

TEST_F(WordAdderZ4Test, AddSymbolAndAddDigitCascadeProperly)
{
    TransitionManager tm = make_tm();
    CayleyTable table = make_table(tm);
    WordAdder adder(table, a);

    EXPECT_EQ(adder.add(Word({d, d, a}), b), (OperationResult{Word({a, a, b})}));
    EXPECT_EQ(adder.add(Word({d, d, d}), b), (OperationResult{Word({a, a, a})}));
    EXPECT_EQ(adder.add_digit(Word({b, d, a}), b, 1), (OperationResult{Word({b, a, b})}));
    EXPECT_EQ(adder.add_digit(Word({b, d, a}), b, 3), (OperationResult{Word({b, d, a})}));
}

TEST_F(WordAdderZ4Test, SupportsWordsLargerThanTableWordSize)
{
    TransitionManager tm = make_tm();
    CayleyTable table = make_table(tm);
    WordAdder adder(table, a);

    Word x({d, d, d, d, d, a});
    Word y({b, a, a, a, a, a});
    Word expected({a, a, a, a, a, b});

    EXPECT_EQ(adder.add(x, y), (OperationResult{expected}));
}

TEST_F(WordAdderZ4Test, SimultaneousCarriesAndOverflowTruncation)
{
    TransitionManager tm = make_tm();
    CayleyTable table = make_table(tm);
    WordAdder adder(table, a);

    EXPECT_EQ(adder.add(Word({d, d, d}), Word({d, d, d})), (OperationResult{Word({c, d, d})}));
    EXPECT_EQ(adder.add(Word({d, d, d}), Word({b, a, a})), (OperationResult{Word({a, a, a})}));
}

TEST(WordAdderComplexGraphTest, DiamondGraphDivergesAndReconvergesViaCarries)
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
    CayleyTable table = TableGenerator(tm, alphabet).generate(AdditionGeneration(tm, 3));
    WordAdder adder(table, a);

    OperationResult split = adder.add(Word({e, b, a}), Word({b, a, a}));
    OperationResult expected_split = {Word({a, c, a}), Word({a, d, a})};
    EXPECT_EQ(split, expected_split);

    OperationResult reconverge = adder.add(Word({e, c, b}), Word({b, b, a}));
    OperationResult expected_reconverge = {Word({a, a, c}), Word({a, a, d})};
    EXPECT_EQ(reconverge, expected_reconverge);
}

TEST(WordAdderComplexGraphTest, AsymmetricCyclesSecondOrderCarryJumpsDigit)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};

    TransitionGraph graph;
    graph.set(a, Term{b});
    graph.set(b, Term{a, c});
    graph.set(c, Term{a});

    TransitionManager tm(std::move(graph), a);
    CayleyTable table = TableGenerator(tm, alphabet).generate(AdditionGeneration(tm, 3));
    WordAdder adder(table, a);

    OperationResult res = adder.add(Word({c, a, a, a}), Word({c, a, a, a}));
    EXPECT_TRUE(res.contains(Word({a, a, b, a})));
    EXPECT_TRUE(res.contains(Word({a, c, a, a})));
}
