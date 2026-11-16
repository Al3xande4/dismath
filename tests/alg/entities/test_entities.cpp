#include <alg/alphabet.hpp>
#include <alg/result.hpp>
#include <alg/symbol.hpp>
#include <alg/term.hpp>
#include <alg/word.hpp>

#include <gtest/gtest.h>

using namespace alg;

TEST(EntitiesTest, SymbolComparison)
{
    Symbol a('a');
    Symbol b('b');
    Symbol a_copy('a');

    EXPECT_EQ(a, a_copy);
    EXPECT_NE(a, b);
    EXPECT_LT(a, b);
    EXPECT_EQ(a.get_representation(), 'a');
}

TEST(EntitiesTest, TermDeduplicationAndMerge)
{
    Symbol a('a');
    Symbol b('b');
    Symbol c('c');

    Term single(a);
    EXPECT_EQ(single.size(), 1);
    EXPECT_TRUE(single.contains(a));

    Term with_duplicates{a, b, a, b};
    EXPECT_EQ(with_duplicates.size(), 2);
    EXPECT_TRUE(with_duplicates.contains(a));
    EXPECT_TRUE(with_duplicates.contains(b));

    Term t2(c);
    with_duplicates.merge(t2);
    EXPECT_EQ(with_duplicates.size(), 3);
    EXPECT_TRUE(with_duplicates.contains(c));
}

TEST(EntitiesTest, WordOperations)
{
    Symbol a('a');
    Symbol b('b');

    Word w(3, a);
    EXPECT_EQ(w.size(), 3);
    EXPECT_EQ(w[0], a);
    EXPECT_EQ(w[1], a);
    EXPECT_EQ(w[2], a);

    w[1] = b;
    EXPECT_EQ(w[1], b);

    Word expected(std::vector<Symbol>{a, b, a});
    EXPECT_EQ(w, expected);

    size_t count = 0;
    for (const auto &sym : w)
    {
        (void)sym;
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST(EntitiesTest, AlphabetContains)
{
    Symbol a('a');
    Symbol b('b');
    Symbol c('c');
    Symbol d('d');

    Alphabet alphabet{a, b, c};
    EXPECT_EQ(alphabet.size(), 3);
    EXPECT_TRUE(alphabet.contains(a));
    EXPECT_TRUE(alphabet.contains(b));
    EXPECT_TRUE(alphabet.contains(c));
    EXPECT_FALSE(alphabet.contains(d));
}

TEST(EntitiesTest, OperationResultSets)
{
    Symbol a('a');
    Symbol b('b');

    SymbolOperationResult sym_results;
    sym_results.insert({a, Word(1, b)});
    sym_results.insert({a, Word(1, b)});
    EXPECT_EQ(sym_results.size(), 1);

    OperationResult word_results;
    word_results.insert(Word(2, a));
    word_results.insert(Word(2, b));
    EXPECT_EQ(word_results.size(), 2);
}
