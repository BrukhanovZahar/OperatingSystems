#include <iostream>
#include <vector>
#include "free_blocks_allocator.h"
#include "binary_allocator.h"
#include <benchmark/benchmark.h>
#include <chrono>

static void BM_FreeBlocksAllocator(benchmark::State& state) {
    for (auto _: state) {
        auto allocator = FreeBlocksAllocator(1024000);
        std::vector<void*> pointers;
        char mod;
        while (std::cin >> mod) {
            if (mod == '+') {
                int size;
                std::cin >> size;
                auto ptr = allocator.allocate(size);
                if (ptr != nullptr) {
                    pointers.push_back(ptr);
                }
            }
            if (mod == '-') {
                int index;
                std::cin >> index;
                allocator.deallocate(pointers[index]);
            }
        }
    }
}

BENCHMARK(BM_FreeBlocksAllocator);

static void BM_BinaryAllocator(benchmark::State& state) {
    for (auto _: state) {
        auto allocator = BinaryAllocator(1024000);
        std::vector<void*> pointers;
        char mod;
        while (std::cin >> mod) {
            if (mod == '+') {
                int size;
                std::cin >> size;
                auto ptr = allocator.allocate(size);
                if (ptr) {
                    pointers.push_back(ptr);
                }
            }
            if (mod == '-') {
                int index;
                std::cin >> index;
                allocator.deallocate(pointers[index]);
            }
        }
    }
}

BENCHMARK(BM_BinaryAllocator);

BENCHMARK_MAIN();