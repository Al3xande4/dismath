#include <alg/word.hpp>

#include <algorithm>

namespace alg
{

bool Word::is_filled_with(const Symbol &fill) const
{
    return std::ranges::all_of(m_word, [&](const Symbol &s) { return s == fill; });
}

bool Word::fits_in(size_t target_size, const Symbol &fill) const noexcept
{
    if (target_size >= m_size)
        return true;

    return std::ranges::all_of(
        m_word.begin() + target_size, m_word.end(), [&fill](const Symbol &s) { return s == fill; }
    );
}

Word Word::shifted_left(size_t offset, const Symbol &fill) const
{
    Word shifted(m_size, fill);
    if (offset < m_size)
        std::ranges::copy_n(m_word.begin(), m_size - offset, shifted.m_word.begin() + offset);

    return shifted;
}

Word Word::shifted_right(size_t offset, const Symbol &fill) const
{
    Word shifted(m_size, fill);
    if (offset < m_size)
        std::ranges::copy_n(m_word.begin() + offset, m_size - offset, shifted.m_word.begin());

    return shifted;
}

Word Word::with_symbol(size_t idx, const Symbol &symbol) const
{
    Word new_word(*this);
    new_word[idx] = symbol;
    return new_word;
}

Word Word::resized(size_t new_size, const Symbol &fill) const
{
    Word result(new_size, fill);
    std::ranges::copy_n(m_word.begin(), std::min(m_size, new_size), result.m_word.begin());
    return result;
}

} // namespace alg
