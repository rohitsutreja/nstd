#include <iostream>
#include "nstd/vector.hpp"

int main()
{
    nstd::vector<int> v{};
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    for (int i{}; i < 3; ++i)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";

    return 0;
}
