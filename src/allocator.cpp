#include <map>
#include <vector>
#include "version.h"
#include "slvr_allocator.h"
#include "slvr_lib_factorial.h"
#include "slvr_container.h"

using namespace slvr::allocator;

int main(int, char *[])
{

    using namespace slvr::lib;
    using namespace slvr::container;
    auto m1 = std::map<int, int>{};
    for (int i = 0; i < 10; ++i)
        m1[i] = llfactorial(i);

    auto m2 = std::map<int, int, std::less<int>, superK<std::pair<int, int>>>{};
    auto all_m2 = m2.get_allocator();
    all_m2.set_limit(10);
    for (int i = 0; i < 10; ++i)
        m2[i] = llfactorial(i);

    for (auto it = m1.cbegin(); it != m1.cend(); ++it)
        std::cout << it->first << ' ' << it->second << '\n';
    std::cout << '\n';

    for (auto it = m2.cbegin(); it != m2.cend(); ++it)
        std::cout << it->first << ' ' << it->second << '\n';
    std::cout << '\n';

    m2.clear();

    massive<int, std::allocator<int>> m3;
    for (int it = 0; it < 10; ++it)
        m3.push_back(it);

    massive<int, superK<int>> m4;
    auto allm4 = m4.get_allocator();
    allm4.set_limit(10);
    for (int it = 0; it < 10; ++it)
        m4.push_back(it);

    for (auto &el : m3)
        std::cout << el << '\n';
    std::cout << '\n';

    for (auto &el : m4)
        std::cout << el << '\n';
    std::cout << '\n';

    return 0;
}
