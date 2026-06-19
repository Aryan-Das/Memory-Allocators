#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <new> 
#include <type_traits>

class PoolResource{
public:
    PoolResource(std::size_t block_size, std::size_t block_count)
        : block_size_{block_size}, 
        block_count_{block_count}, 
        chunk{new std::byte[block_count * block_size]}, 
        free_list_{nullptr} 
    {
        for (size_t i{0}; i < block_count; ++i){
            auto* block = reinterpret_cast<FreeNode*>(chunk + (i * block_size));

            block->next = free_list_;
            free_list_ = block;
        }
    }

    PoolResource(const PoolResource& other) = delete;
    PoolResource& operator=(const PoolResource& other) = delete;

    void* allocate(){
        if(free_list_){
            FreeNode* block = free_list_;
            free_list_ = block->next;
            return static_cast<void*>(block);
        }else{
            throw std::bad_alloc();
        }
    }

    void deallocate(void* ptr) noexcept{
        FreeNode* block = static_cast<FreeNode*>(ptr);
        block->next = free_list_;
        free_list_ = block;
    }

    std::size_t block_size(){
        return block_size_;
    }
    std::size_t block_count(){
        return block_count_;
    }

    ~PoolResource(){
        delete[] chunk;
    }

private:
    struct FreeNode{
        FreeNode* next;
    };
    std::byte* chunk;
    std::size_t block_size_;
    std::size_t block_count_;
    FreeNode* free_list_;
};


template <typename T>
class PoolAllocator {
public:
    using value_type = T;
    using is_always_equal = std::false_type;

    explicit PoolAllocator(PoolResource* resource) noexcept : resource_{resource} {}
    
    template <typename U>
    PoolAllocator(const PoolAllocator<U>& other) : resource_{other.resource()} {}

    PoolResource* resource() const noexcept {
        return resource_;
    }

    [[nodiscard]] T* allocate(std::size_t n){
        if(n != 1){
            throw std::bad_alloc();
        }
        return static_cast<T*>(resource_->allocate());
    }


    void deallocate(T* ptr, std::size_t n) noexcept {
        resource_->deallocate(static_cast<void*>(ptr));
    }

    
    template <typename U>
    bool operator==(const PoolAllocator<U>& other) const noexcept {
        return resource_ == other.resource();
    }

    template <typename U>
    bool operator!=(const PoolAllocator<U>& other) const noexcept {
        return resource_ != other.resource();
    }

    template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };



private:
    PoolResource* resource_;
};


#endif