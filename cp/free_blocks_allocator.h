#pragma once

#include "allocator.h"

class FreeBlocksAllocator : public Allocator {
public:

    explicit FreeBlocksAllocator(size_type size) {
        if ((startPointer = malloc(size)) == nullptr) {
            std::cerr << "Failed to allocate memory\n";
            return;
        }
        totalSize = size;
        endPointer = static_cast<void*>(static_cast<char*>(startPointer) + totalSize);
        auto* header = (Header*) startPointer;
        header->isAvailable = true;
        header->size = (totalSize - headerSize);
        header->previousSize = 0;
        usedSize = headerSize;
    };

    pointer allocate(size_type size) override {
        if (size <= 0) {
            std::cerr << "Size must be bigger than 0\n";
            return nullptr;
        }
        if (size > totalSize - usedSize) { return nullptr; }
        auto* header = find(size);
        if (header == nullptr) { return nullptr; }
        splitBlock(header, size);
        return header + 1;
    };

    void deallocate(pointer ptr) override {
        if (!validateAddress(ptr)) {
            return;
        }
        auto* header = static_cast<Header*>(ptr) - 1;
        header->isAvailable = true;
        usedSize -= header->size;
        defragmentation(header);
    };
};