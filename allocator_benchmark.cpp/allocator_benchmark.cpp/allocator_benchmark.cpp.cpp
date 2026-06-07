#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <algorithm>

class Allocator
{
public:
    virtual void* Allocate(size_t size, size_t alignment = 8) = 0;
    virtual void Free(void* ptr) = 0;
    virtual ~Allocator() {}
};

class CAllocator : public Allocator
{
public:
    void* Allocate(size_t size, size_t alignment = 8) override
    {
        return malloc(size);
    }

    void Free(void* ptr) override
    {
        free(ptr);
    }
};

class LinearAllocator : public Allocator
{
private:
    char* memory;
    size_t totalSize;
    size_t offset;

public:
    LinearAllocator(size_t size)
    {
        memory = new char[size];
        totalSize = size;
        offset = 0;
    }

    void* Allocate(size_t size, size_t alignment = 8) override
    {
        if (offset + size > totalSize)
            return nullptr;

        void* ptr = memory + offset;
        offset += size;
        return ptr;
    }

    void Free(void* ptr) override
    {
    }

    ~LinearAllocator()
    {
        delete[] memory;
    }
};

class StackAllocator : public Allocator
{
private:
    char* memory;
    size_t totalSize;
    size_t offset;

public:
    StackAllocator(size_t size)
    {
        memory = new char[size];
        totalSize = size;
        offset = 0;
    }

    void* Allocate(size_t size, size_t alignment = 8) override
    {
        if (offset + size > totalSize)
            return nullptr;

        void* ptr = memory + offset;
        offset += size;
        return ptr;
    }

    void Free(void* ptr) override
    {
    }

    ~StackAllocator()
    {
        delete[] memory;
    }
};

class PoolAllocator : public Allocator
{
private:
    std::vector<void*> pool;
    size_t chunkSize;

public:
    PoolAllocator(size_t totalSize, size_t chunk)
    {
        chunkSize = chunk;

        size_t count = totalSize / chunk;

        for (size_t i = 0; i < count; i++)
        {
            pool.push_back(malloc(chunkSize));
        }
    }

    void* Allocate(size_t size, size_t alignment = 8) override
    {
        if (pool.empty())
            return nullptr;

        void* ptr = pool.back();
        pool.pop_back();
        return ptr;
    }

    void Free(void* ptr) override
    {
        pool.push_back(ptr);
    }

    ~PoolAllocator()
    {
        for (void* ptr : pool)
        {
            free(ptr);
        }
    }
};

class FreeListAllocator : public Allocator
{
public:
    void* Allocate(size_t size, size_t alignment = 8) override
    {
        return malloc(size);
    }

    void Free(void* ptr) override
    {
        free(ptr);
    }
};

class Benchmark
{
private:
    int operations;

public:
    Benchmark(int ops)
    {
        operations = ops;
    }

    void MultipleAllocation(Allocator* allocator,
        const std::vector<size_t>& sizes,
        const std::vector<size_t>& alignments)
    {
        std::vector<void*> ptrs;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < operations; i++)
        {
            for (size_t j = 0; j < sizes.size(); j++)
            {
                ptrs.push_back(
                    allocator->Allocate(sizes[j], alignments[j])
                );
            }
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Multiple Allocation Time: "
            << diff.count()
            << " sec"
            << std::endl;

        for (void* ptr : ptrs)
        {
            allocator->Free(ptr);
        }
    }

    void MultipleFree(Allocator* allocator,
        const std::vector<size_t>& sizes,
        const std::vector<size_t>& alignments)
    {
        std::vector<void*> ptrs;

        for (int i = 0; i < operations; i++)
        {
            for (size_t j = 0; j < sizes.size(); j++)
            {
                ptrs.push_back(
                    allocator->Allocate(sizes[j], alignments[j])
                );
            }
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (void* ptr : ptrs)
        {
            allocator->Free(ptr);
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Multiple Free Time: "
            << diff.count()
            << " sec"
            << std::endl;
    }

    void RandomAllocation(Allocator* allocator,
        const std::vector<size_t>& sizes,
        const std::vector<size_t>& alignments)
    {
        std::vector<void*> ptrs;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < operations; i++)
        {
            size_t index = rand() % sizes.size();

            ptrs.push_back(
                allocator->Allocate(
                    sizes[index],
                    alignments[index]
                )
            );
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Random Allocation Time: "
            << diff.count()
            << " sec"
            << std::endl;

        for (void* ptr : ptrs)
        {
            allocator->Free(ptr);
        }
    }

    void RandomFree(Allocator* allocator,
        const std::vector<size_t>& sizes,
        const std::vector<size_t>& alignments)
    {
        std::vector<void*> ptrs;

        for (int i = 0; i < operations; i++)
        {
            size_t index = rand() % sizes.size();

            ptrs.push_back(
                allocator->Allocate(
                    sizes[index],
                    alignments[index]
                )
            );
        }

        std::random_shuffle(ptrs.begin(), ptrs.end());

        auto start = std::chrono::high_resolution_clock::now();

        for (void* ptr : ptrs)
        {
            allocator->Free(ptr);
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Random Free Time: "
            << diff.count()
            << " sec"
            << std::endl;
    }

    void SingleAllocation(Allocator* allocator,
        size_t size,
        size_t alignment)
    {
        auto start = std::chrono::high_resolution_clock::now();

        void* ptr = allocator->Allocate(size, alignment);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Single Allocation Time: "
            << diff.count()
            << " sec"
            << std::endl;

        allocator->Free(ptr);
    }

    void SingleFree(Allocator* allocator,
        size_t size,
        size_t alignment)
    {
        void* ptr = allocator->Allocate(size, alignment);

        auto start = std::chrono::high_resolution_clock::now();

        allocator->Free(ptr);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> diff = end - start;

        std::cout << "Single Free Time: "
            << diff.count()
            << " sec"
            << std::endl;
    }
};

int main()
{
    const size_t A = 100000000;
    const size_t B = 10000000;

    const std::vector<size_t> ALLOCATION_SIZES
    {
        32,
        64,
        256,
        512,
        1024,
        2048,
        4096
    };

    const std::vector<size_t> ALIGNMENTS
    {
        8, 8, 8, 8, 8, 8, 8
    };

    const int OPERATIONS = 5000;

    Allocator* cAllocator = new CAllocator();
    Allocator* linearAllocator = new LinearAllocator(A);
    Allocator* stackAllocator = new StackAllocator(A);
    Allocator* poolAllocator = new PoolAllocator(16777216, 4096);
    Allocator* freeListAllocator = new FreeListAllocator();

    Benchmark benchmark(OPERATIONS);

    std::cout << "C" << std::endl;
    benchmark.MultipleAllocation(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    std::cout << std::endl;

    std::cout << "LINEAR" << std::endl;
    benchmark.MultipleAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    std::cout << std::endl;

    std::cout << "STACK" << std::endl;
    benchmark.MultipleAllocation(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    std::cout << std::endl;

    std::cout << "POOL" << std::endl;
    benchmark.SingleAllocation(poolAllocator, 4096, 8);
    benchmark.SingleFree(poolAllocator, 4096, 8);

    std::cout << std::endl;

    std::cout << "FREE LIST" << std::endl;
    benchmark.MultipleAllocation(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    delete cAllocator;
    delete linearAllocator;
    delete stackAllocator;
    delete poolAllocator;
    delete freeListAllocator;

    return 0;
}