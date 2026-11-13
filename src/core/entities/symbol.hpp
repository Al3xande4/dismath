#pragma once

#include <compare>

class Symbol
{
private:
    char m_representation;

public:
    Symbol(char representation) : m_representation(representation) {}

    char get_representation() const { return m_representation; }

    bool operator==(const Symbol &) const = default;
    auto operator<=>(const Symbol &) const = default;
};