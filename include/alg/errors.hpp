#pragma once

#include <stdexcept>

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

class AxiomError : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class TransitionError : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

class DivisionByZero : public AlgebraError
{
public:
    using AlgebraError::AlgebraError;
};

} // namespace alg
