#pragma once

#include <alg/symbol.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace alg
{

class AlgebraError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class SizesMismatch : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class WordSizeError : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class DivisionByZero : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class InvalidSymbol : public AlgebraError
{
private:
    Symbol m_symbol;

public:
    InvalidSymbol(const std::string &msg, Symbol symbol = Symbol('\0')) : AlgebraError(msg), m_symbol(symbol) {}

    Symbol symbol() const noexcept { return m_symbol; }
};

class InvalidZero : public AlgebraError
{
private:
    Symbol m_zero;

public:
    InvalidZero(Symbol zero, const std::string &msg) : AlgebraError(msg), m_zero(zero) {}
    Symbol zero() const noexcept { return m_zero; }
};

class ZeroNotInAlphabet : public InvalidZero
{
public:
    explicit ZeroNotInAlphabet(Symbol zero) : InvalidZero(zero, "Zero symbol must belong to the alphabet") {}
};

class GraphError : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class EmptyAlphabet : public GraphError
{
public:
    EmptyAlphabet() : GraphError("Alphabet cannot be empty") {}
};

class NodeError : public GraphError
{
private:
    Symbol m_node;

public:
    NodeError(Symbol node, const std::string &msg) : GraphError(msg), m_node(node) {}
    Symbol node() const noexcept { return m_node; }
};

class EdgeError : public NodeError
{
private:
    Symbol m_target;

public:
    EdgeError(Symbol source, Symbol target, const std::string &msg) : NodeError(source, msg), m_target(target) {}
    Symbol target() const noexcept { return m_target; }
};

class MissingTransition : public NodeError
{
public:
    explicit MissingTransition(Symbol node) : NodeError(node, "Missing transition rule for this symbol") {}
};

class EmptyTransition : public NodeError
{
public:
    explicit EmptyTransition(Symbol node) : NodeError(node, "Transition term cannot be empty (cannot be '-')") {}
};

class TargetNotInAlphabet : public EdgeError
{
public:
    TargetNotInAlphabet(Symbol source, Symbol target)
        : EdgeError(source, target, "Transition target does not belong to the alphabet")
    {
    }
};

class InvalidOne : public NodeError
{
public:
    using NodeError::NodeError;
};

class AmbiguousOne : public InvalidOne
{
public:
    explicit AmbiguousOne(Symbol zero)
        : InvalidOne(zero, "Zero must have exactly one transition (to One), but has multiple")
    {
    }
};

class DegenerateOne : public InvalidOne
{
public:
    explicit DegenerateOne(Symbol zero) : InvalidOne(zero, "One cannot be equal to Zero (self-loop on zero)") {}
};

class TrappedNodes : public GraphError
{
private:
    std::vector<Symbol> m_trapped;

public:
    TrappedNodes(std::vector<Symbol> trapped)
        : GraphError("Some nodes cannot reach Zero (isolated cycle detected)"), m_trapped(std::move(trapped))
    {
    }

    const std::vector<Symbol> &trapped() const noexcept { return m_trapped; }
};

} // namespace alg