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

namespace
{

alg::TransitionGraph build_z4_graph()
{
    return {
        {alg::Symbol('a'), alg::Term{alg::Symbol('b')}},
        {alg::Symbol('b'), alg::Term{alg::Symbol('c')}},
        {alg::Symbol('c'), alg::Term{alg::Symbol('d')}},
        {alg::Symbol('d'), alg::Term{alg::Symbol('a')}}
    };
}

} // namespace

TEST(AlgebraApiErgonomicsTest, EndToEndClientWorkflowWithoutInternalHeaders)
{
    alg::Alphabet alphabet{'a', 'b', 'c', 'd'};
    alg::Algebra alg(build_z4_graph(), alphabet, alg::Symbol('a'), 3);

    alg::Word x = alg.make_word({'d', 'c'});
    alg::Word y = alg.parse_word("d");

    EXPECT_EQ(x, alg::Word({alg::Symbol('d'), alg::Symbol('c'), alg::Symbol('a')}));
    EXPECT_EQ(y, alg::Word({alg::Symbol('d'), alg::Symbol('a'), alg::Symbol('a')}));
    EXPECT_EQ(alg.zero_word(), alg::Word({alg::Symbol('a'), alg::Symbol('a'), alg::Symbol('a')}));
    EXPECT_EQ(alg.one_word(), alg::Word({alg::Symbol('b'), alg::Symbol('a'), alg::Symbol('a')}));

    alg::OperationResult sum = alg.add(x, y);
    alg::OperationResult prod = alg.multiply(sum, {alg.one_word()});
    alg::DivModResult dm = alg.divmod(x, y);

    EXPECT_EQ(prod, sum);
    EXPECT_EQ(
        dm,
        (alg::DivModResult{
            {alg::Word({alg::Symbol('d'), alg::Symbol('a'), alg::Symbol('a')}),
             alg::Word({alg::Symbol('c'), alg::Symbol('a'), alg::Symbol('a')})}
        })
    );
}

TEST(AlgebraApiErgonomicsTest, FactoryMethodsValidateUserInput)
{
    alg::Alphabet alphabet{'a', 'b', 'c', 'd'};
    alg::Algebra alg(build_z4_graph(), alphabet, alg::Symbol('a'), 3);

    EXPECT_THROW(alg.make_word({'a', 'b', 'c', 'd'}), alg::SizesMismatch);
    EXPECT_THROW(alg.make_word({'z'}), alg::InvalidSymbol);
    EXPECT_THROW(alg.parse_word("abz"), alg::InvalidSymbol);
    EXPECT_THROW(alg.parse_word("abcd"), alg::SizesMismatch);
}

TEST(AlgebraApiErgonomicsTest, AlgebraIsMovableAcrossApplicationLayers)
{
    alg::Alphabet alphabet{'a', 'b', 'c', 'd'};
    alg::Algebra source(build_z4_graph(), alphabet, alg::Symbol('a'), 2);

    alg::Algebra moved = std::move(source);

    alg::Word a = moved.make_word({'c'});
    alg::Word b = moved.make_word({'c'});
    EXPECT_EQ(moved.add(a, b), (alg::OperationResult{moved.make_word({'a', 'b'})}));
}

TEST(AlgebraApiErgonomicsTest, SetOperationsChainCleanlyForExpressionEvaluators)
{
    alg::Alphabet alphabet{'a', 'b', 'c', 'd', 'e'};
    alg::TransitionGraph diamond{
        {alg::Symbol('a'), alg::Term{alg::Symbol('b')}},
        {alg::Symbol('b'), alg::Term{alg::Symbol('c'), alg::Symbol('d')}},
        {alg::Symbol('c'), alg::Term{alg::Symbol('e')}},
        {alg::Symbol('d'), alg::Term{alg::Symbol('e')}},
        {alg::Symbol('e'), alg::Term{alg::Symbol('a')}}
    };

    alg::Algebra alg(std::move(diamond), alphabet, alg::Symbol('a'), 2);

    alg::OperationResult branch = alg.add(alg.make_word({'b'}), alg.make_word({'b'}));
    alg::OperationResult scaled = alg.multiply(branch, alg.make_word({'c'}));
    alg::OperationResult shifted = alg.subtract(scaled, alg.make_word({'b'}));

    EXPECT_EQ(branch, (alg::OperationResult{alg.make_word({'c'}), alg.make_word({'d'})}));
    EXPECT_FALSE(shifted.empty());
}