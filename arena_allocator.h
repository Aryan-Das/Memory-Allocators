#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <new> 
#include <type_traits>

class ArenaResource{
public:
    ArenaResource(std::size_t capacity) : chunk{new std::byte[capacity]}, capacity_{capacity}, offset{0} {}
    ~ArenaResource(){
        delete[] chunk;
    }

    ArenaResource(const ArenaResource& other) = delete;
    ArenaResource& operator=(const ArenaResource& other) = delete;

   
    void* allocate(std::size_t bytes, std::size_t alignment) {
        void* current = chunk + offset;
        std::size_t space = capacity_ - offset;
        void* allocated = std::align(alignment, bytes, current, space);
        if (allocated){
            offset = (capacity_ - space) + bytes;
            return allocated;
        }else{
            throw std::bad_alloc();
        }
    }

    void reset() noexcept{
        offset = 0;
    }
    std::size_t capacity() const noexcept{
        return capacity_;
    }
    std::size_t used() const noexcept{
        return offset;
    }


private:
    std::byte* chunk;
    std::size_t capacity_;
    std::size_t offset;

};

template <typename T>
class ArenaAllocator {
public:
    using value_type = T;
    using is_always_equal = std::false_type;

    explicit ArenaAllocator(ArenaResource* resource) noexcept : resource_{resource} {}
    
    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) : resource_{other.resource()} {}

    ArenaResource* resource() const noexcept {
        return resource_;
    }

    [[nodiscard]] T* allocate(std::size_t n){
        return static_cast<T*>(resource_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept {} //deallocate actually does nothing, since the arena resource owns the memory and delets it upon destruction. No memberwise free
    



private:
    ArenaResource* resource_;
};

#endif