#pragma once

#include <alg/result.hpp>
#include <alg/symbol.hpp>

#include <map>
#include <utility>

namespace alg
{

class CayleyTable
{
private:
    std::map<std::pair<Symbol, Symbol>, OperationResult> m_table;

public:
    bool contains(const Symbol &x, const Symbol &y) const { return m_table.contains({x, y}); }

    const OperationResult &get(const Symbol &x, const Symbol &y) const { return m_table.at({x, y}); }

    void set(const Symbol &x, const Symbol &y, OperationResult result) { m_table[{x, y}] = std::move(result); }

    bool merge(const Symbol &x, const Symbol &y, const OperationResult &new_results)
    {
        auto &current = m_table[{x, y}];

        bool has_new = false;
        for (const auto &item : new_results)
            if (current.insert(item).second)
                has_new = true;

        return has_new;
    }
};

} // namespace alg
