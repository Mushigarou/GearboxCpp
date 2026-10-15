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
    Pool<std::string>::Object obj = p.acquire("heelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheelooheeloo");
    Pool<std::string>::Object obj2 = p.acquire("heeloos1");
    Pool<std::string>::Object obj3 = p.acquire("heeloos2");
    Pool<std::string>::Object obj4 = p.acquire("heeloos3");
    Pool<std::string>::Object obj5 = p.acquire("heeloos4");
    p.resize(10);
    Pool<std::string>::Object obj6 = p.acquire("heeloos5");
    Pool<std::string>::Object obj7 = p.acquire("heeloos6");
    Pool<std::string>::Object obj8 = p.acquire("heeloos7");
    Pool<std::string>::Object obj9 = p.acquire("heeloos8");
    Pool<std::string>::Object obj10 = p.acquire("heeloos9");
    std::cout << obj->data()
    << " " << obj2->data()
    << " " << obj3->data()
    << " " << obj4->data()
    << " " << obj5->data()
    << " " << obj6->data()
    << " " << obj7->data()
    << " " << obj8->data()
    << " " << obj9->data()
    << " " << obj10->data()
    << std::endl;
}


int main() {
    createPair();
    createVector();
    createString();

    return 0;
}