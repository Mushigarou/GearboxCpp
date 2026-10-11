#include <cstddef>
#include <iostream>
#include "data-structures/pool/pool.hpp"
#include <vector>

int main() {
    Pool<std::vector<int>> p = Pool<std::vector<int>>(10);

    // std::vector<int>(10, 9);
    Pool<std::vector<int>>::Object obj1 = p.acquire(1, 1);
    
    // for (const auto &num: obj1) {
    //     std::cout << num;
    // }
    

    // p.resize(-19);
    // (void)p;
    std::cout << obj1->size() << std::endl;
    return 0;
}