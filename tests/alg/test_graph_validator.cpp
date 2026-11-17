#include <alg/alphabet.hpp>
#include <alg/errors.hpp>
#include <alg/graph_validator.hpp>
#include <alg/symbol.hpp>
#include <alg/term.hpp>
#include <alg/transition_graph.hpp>

#include <gtest/gtest.h>

using namespace alg;

TEST(GraphValidatorTest, ValidGraphPasses)
{
    Symbol a('a'), b('b'), c('c'), d('d');
    Alphabet alphabet{a, b, c, d};
    TransitionGraph graph{{a, Term{b}}, {b, Term{c}}, {c, Term{d}}, {d, Term{a}}};

    EXPECT_NO_THROW(GraphValidator::validate(graph, alphabet, a));
}

TEST(GraphValidatorTest, RejectsEmptyAlphabetOrForeignZero)
{
    Symbol a('a'), z('z');
    TransitionGraph empty_graph;

    EXPECT_THROW(GraphValidator::validate(empty_graph, Alphabet{}, a), EmptyAlphabet);
    EXPECT_THROW(GraphValidator::validate(empty_graph, Alphabet{a}, z), ZeroNotInAlphabet);
}

TEST(GraphValidatorTest, RejectsStructureErrors)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};

    TransitionGraph missing{{a, Term{b}}, {b, Term{a}}};
    EXPECT_THROW(GraphValidator::validate(missing, alphabet, a), MissingTransition);

    TransitionGraph empty_term{{a, Term{b}}, {b, Term{c}}, {c, Term{}}};
    EXPECT_THROW(GraphValidator::validate(empty_term, alphabet, a), EmptyTransition);

    Symbol z('z');
    TransitionGraph foreign_target{{a, Term{b}}, {b, Term{c}}, {c, Term{z}}};
    EXPECT_THROW(GraphValidator::validate(foreign_target, alphabet, a), TargetNotInAlphabet);
}

TEST(GraphValidatorTest, RejectsUnitErrors)
{
    Symbol a('a'), b('b'), c('c');
    Alphabet alphabet{a, b, c};

    TransitionGraph ambiguous{{a, Term{b, c}}, {b, Term{a}}, {c, Term{a}}};
    EXPECT_THROW(GraphValidator::validate(ambiguous, alphabet, a), AmbiguousOne);

    TransitionGraph degenerate{{a, Term{a}}, {b, Term{c}}, {c, Term{b}}};
    EXPECT_THROW(GraphValidator::validate(degenerate, alphabet, a), DegenerateOne);
}

TEST(GraphValidatorTest, RejectsTrappedNodesInIsolatedCycles)
{
    Symbol a('a'), b('b'), c('c'), d('d'), e('e');
    Alphabet alphabet{a, b, c, d, e};

    TransitionGraph trapped{{a, b}, {b, {c, d}}, {c, e}, {d, a}, {e, c}};

    try
    {
        GraphValidator::validate(trapped, alphabet, a);
        FAIL() << "Expected TrappedNodes exception";
    }
    catch (const TrappedNodes &error)
    {
        EXPECT_EQ(error.trapped().size(), 2);
        EXPECT_TRUE(std::ranges::find(error.trapped().begin(), error.trapped().end(), c) != error.trapped().end());
        EXPECT_TRUE(std::ranges::find(error.trapped().begin(), error.trapped().end(), e) != error.trapped().end());
    }
    catch (...)
    {
        FAIL() << "Expected TrappedNodes exception, got another";
    }
}