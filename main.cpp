#include <cstddef>
#include <iostream>
#include "data-structures/pool/pool.hpp"
#include <vector>

void  createPair() {
    Pool<std::pair<int, char>> p = Pool<std::pair<int, char>>(5);
    Pool<std::pair<int, char>>::Object obj = p.acquire(111, 199);
    std::cout << obj->first << std::endl;
}

void  createVector() {
    Pool<std::vector<int>> p = Pool<std::vector<int>>(5);
    Pool<std::vector<int>>::Object obj = p.acquire(10, 10);
    std::cout << obj->size() << std::endl;
}


void  createString() {
    Pool<std::string> p = Pool<std::string>(5);
    Pool<std::string>::Object obj = p.acquire("heeloo");
    Pool<std::string>::Object obj2 = p.acquire("heeloos");
    std::cout << obj->data() << " " << obj2->data() << std::endl;
}


int main() {
    createPair();
    createVector();
    createString();

    return 0;
}