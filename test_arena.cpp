#include "arena_allocator.h"
#include <vector>
#include <iostream>

int main(){
    ArenaResource resource{4096};
    ArenaAllocator<int> allocator{&resource};
    std::vector<int, ArenaAllocator<int>> v(allocator);
    for(int i = 0; i <= 10; ++i){
        v.push_back(i * 10);
    }
    for(int i : v){
        std::cout << i << std::endl;
    }
    std::cout << resource.used();
}