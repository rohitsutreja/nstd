#include <algorithm>
#include <functional>
#include <iterator>

namespace nstd {
template<typename Iterator, Comparator>
Iterator lomuto_partition(Iterator begin, Iterator end, Comparator comp) {
    auto last = std::prev(end);
    auto pivot = last;

    auto left = begin;

    for (auto it = begin; it != last; ++it) {
        if (comp(*it, *pivot)) {
            if (it != left) {
                std::iter_swap(it, left);
            }
            ++left;
        }
    }

    std::iter_swap(left, pivot);

    return left;
}

template<typename Iterator, typename Comparator>
Iterator hoare_partition(Iterator begin, Iterator end, Comparator comp) {
    auto pivot = *std::next(begin, std::distance(begin, end) / 2);

    auto i = begin;
    auto j = std::prev(end);

    while (true) {
        while (comp(*i, pivot)) {
            ++i;
        }

        while (comp(pivot, *j)) {
            --j;
        }

        if (i >= j) {
            return j;
        }

        std::iter_swap(i, j);
        ++i;
        --j;
    }
}

template<typename Iterator, typename Comparator>
Iterator hoare_partition(Iterator begin, Iterator end, Comparator comp) {
    auto i = begin;
    auto j = std::prev(end);

    auto pivot = *j;

    while (i <= j) {
        while (*i < pivot) {
            ++i;
        }

        while (*j > pivot) {
            ++j;
        }

        if (i < j) {
            std::iter_swap(i, j);
        }
    }

    return j;
}

template<typename Iterator, typename Comparator>
void quick_sort(Iterator begin, Iterator end, Comparator comp) {
    if (begin == end || std::next(begin) == end) {
        return;
    }

    auto pi = lomuto_partition(begin, end, comp);

    nstd::quick_sort(begin, pi, comp);
    nstd::quick_sort(std::next(pi), end, comp);
}

template<typename Iterator> void sort(Iterator begin, Iterator end) {
    using T = typename std::iterator_traits<Iterator>::value_type;
    nstd::quick_sort(begin, end, std::less<T>());
}
template<typename Iterator, typename Comparator>
void sort(Iterator begin, Iterator end, Comparator comp) {
    nstd::quick_sort(begin, end, comp);
}
} // namespace nstd