#include "arena_allocator.h"
#include "pool_allocator.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <new>

struct Particle{
    int x, y, vx, vy;
};

volatile std::size_t sink;

long long benchmark_pool(std::size_t frames, std::size_t particles_per_frame){
    PoolResource pool{sizeof(Particle), particles_per_frame};
    auto start = std::chrono::steady_clock::now();
    
    for(int frame = 0; frame < frames; ++frame){
        std::vector<Particle*> particles;
        particles.reserve(particles_per_frame);

        for(int i = 0; i < particles_per_frame; ++i){
            auto* mem = pool.allocate();
            auto* p = new (mem) Particle{i, frame, 1, -1, }; 
            particles.push_back(p);

        }
        for(Particle*& p : particles){
            p->x += p->vx;
            p->y += p-> vy;
            sink += p->x;
        }
        for(Particle*& p : particles){
            p->~Particle(); 
            pool.deallocate(static_cast<void*>(p)); 
        }

        
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}
