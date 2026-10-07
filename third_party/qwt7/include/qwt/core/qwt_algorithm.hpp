#ifndef QWT_ALGORITHM_HPP
#define QWT_ALGORITHM_HPP
#include <algorithm>  // std::find
#include <iterator>   // std::prev

/**
 * @brief Find the iterator to the next element in a range with circular wrapping
 *
 * Searches the iterator range [begin, end) for the next element relative to
 * the given current element and direction (forward/backward).
 * If the current element is not in the range, counting starts from the first element.
 * If the range is empty, returns the end iterator.
 * Supports bidirectional iterators (e.g. QList, std::vector, std::list).
 *
 * @tparam Iter Bidirectional iterator type
 * @param begin Start of the range (inclusive)
 * @param end End of the range (exclusive)
 * @param current Current element to compute the next element from
 * @param forward Direction flag: true for forward (++), false for backward (--)
 * @return Iterator to the next element; returns end if the range is empty
 *
 * @note 1. If current is not in [begin, end), counting starts from begin
 *       2. Wraps around: forward past end goes to begin; backward past begin goes to prev(end)
 *       3. Element type must support == comparison (used by std::find)
 *
 * @code
 * std::vector<int> nums = {10, 20, 30, 40};
 * int current = 20;
 *
 * // Forward: 20 -> 30
 * auto it = qwtSelectNextIterator(nums.begin(), nums.end(), current, true);
 *
 * // Backward: 20 -> 10
 * it = qwtSelectNextIterator(nums.begin(), nums.end(), current, false);
 * @endcode
 */
template< typename Iter >
Iter qwtSelectNextIterator(Iter begin, Iter end, typename std::iterator_traits< Iter >::value_type current, bool forward)
{
    if (begin == end) {
        return end;
    }

    Iter ite = std::find(begin, end, current);

    if (ite == end) {
        ite = begin;
    }

    if (forward) {
        ++ite;
        if (ite == end) {
            ite = begin;
        }
    } else {
        if (ite != begin) {
            --ite;
        } else {
            ite = std::prev(end);
        }
    }

    return ite;
}

#endif  // QWT_ALGORITHM_HPP
