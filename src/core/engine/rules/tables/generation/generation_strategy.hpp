#pragma once

#include "core/entities/result.hpp"
#include "core/entities/symbol.hpp"

class IGenerationStrategy
{
public:
    virtual ~IGenerationStrategy() = default;

    virtual bool is_axiom(const Symbol &x, const Symbol &y) const = 0;
    virtual OperationResult axiom_value(const Symbol &x, const Symbol &y) const = 0;

    virtual OperationResult advance(const OperationResult &prev, const Symbol &fixed) const = 0;
};