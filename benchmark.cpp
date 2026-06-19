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

long long benchmark_arena(std::size_t frames, std::size_t particles_per_frame){
    ArenaResource arena(particles_per_frame * sizeof(Particle) + 1024);    
    auto start = std::chrono::steady_clock::now();
    for(int frame = 0; frame < frames; ++frame){
        arena.reset();
        std::vector<Particle*> particles;
        particles.reserve(particles_per_frame);

        for(int i = 0; i < particles_per_frame; ++i){
            auto* mem = arena.allocate(sizeof(Particle), alignof(Particle));
            auto* p = new (mem) Particle{i, frame, 1, -1, }; 
            particles.push_back(p);

        }
        for(Particle*& p : particles){
            p->x += p->vx;
            p->y += p-> vy;
            sink += p->x;
        }
        
        
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

}
long long benchmark_new_delete(std::size_t frames, std::size_t particles_per_frame){
    auto start = std::chrono::steady_clock::now();
    for(int frame = 0; frame < frames; ++frame){
        std::vector<Particle*> particles;
        particles.reserve(particles_per_frame);

        for(int i = 0; i < particles_per_frame; ++i){
            auto* p = new Particle{i, frame, 1, -1, }; 
            particles.push_back(p);

        }
        for(Particle*& p : particles){
            p->x += p->vx;
            p->y += p-> vy;
            sink += p->x;
        }
        for(Particle*& p : particles){
            delete p;
        }
    }
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}



int main(){
    constexpr std::size_t frames = 1000;
    constexpr std::size_t particles_per_frame = 500;
    constexpr int runs = 5;

    long long total_new = 0, total_pool = 0, total_arena = 0;

    for(int i = 0; i < runs; ++i){
        total_new += benchmark_new_delete(frames, particles_per_frame);
        total_pool += benchmark_pool(frames, particles_per_frame);
        total_arena += benchmark_arena(frames, particles_per_frame);
    }

    long long avg_new = total_new / runs;
    long long avg_pool = total_pool / runs;
    long long avg_arena = total_arena / runs;

    double pool_speedup = static_cast<double>(avg_new) / static_cast<double>(avg_pool);
    double arena_speedup = static_cast<double>(avg_new) / static_cast<double>(avg_arena);

    std::cout << "Average New and Delete Time: " << avg_new << std::endl;
    std::cout << "Average Arena Allocator Time: " << avg_arena << std::endl;
    std::cout << "Average Pool Allocator Time: " << avg_pool << std::endl;

    std::cout << "Pool Speed Increase: " << pool_speedup << std::endl;
    std::cout << "Arena Speed Increase: " << arena_speedup << std::endl;

    return 0;
}