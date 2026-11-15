#pragma once

#include <stdexcept>
#include <string>

class SizesMismatch : public std::invalid_argument
{
public:
    SizesMismatch(const std::string &msg) : std::invalid_argument(msg) {}
};

class AxiomError : public std::invalid_argument
{
public:
    AxiomError(const std::string &msg) : std::invalid_argument(msg) {}
};

class TransitionError : public std::invalid_argument
{
public:
    TransitionError(const std::string &msg) : std::invalid_argument(msg) {}
};
class DivisionByZero : public std::invalid_argument
{
public:
    DivisionByZero(const std::string &msg) : std::invalid_argument(msg) {}
};