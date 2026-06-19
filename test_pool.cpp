#include "pool_allocator.h"
#include <list>
#include <iostream>

int main(){
    PoolResource resource{64, 100};
    PoolAllocator<int> allocator{&resource};

    std::list<int, PoolAllocator<int>> l(allocator);

    for(int i = 0; i < 10; ++i){
        l.push_back(i);
    }
    for(int i: l){
        std::cout << i;
    }

}