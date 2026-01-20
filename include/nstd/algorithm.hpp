namespace nstd
{
    template <typename Iterator, typename Comparator>
    void quick_sort(Iterator begin, Iterator end, Comparator comp)
    {
        if (begin == end || std::next(begin) == end)
        {
            return;
        }

        auto pivot = std::move(*(end - 1));

        auto left = begin;
        for (auto it = begin; it != (end - 1); ++it)
        {
            if (comp(*it, pivot))
            {
                if (it != left)
                {
                    std::swap(*left, *it);
                }
                ++left;
            }
        }

        *(end - 1) = std::move(*left);
        *left = std::move(pivot);

        nstd::quick_sort(begin, left, comp);
        nstd::quick_sort(left + 1, end, comp);
    }

    template <typename Iterator, typename Comparator>
    void sort(Iterator begin, Iterator end, Comparator comp)
    {
        nstd::quick_sort(begin, end, comp);
    }
}