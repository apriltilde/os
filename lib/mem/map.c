#include <stdint.h>
#include "../core/io.h"

#define PAGE_PRESENT    0x1
#define PAGE_RW         0x2
#define PAGE_USER       0x4
#define PAGE_SIZE       4096
#define PAGE_ENTRIES    1024

// Page directory: 1024 entries, each maps 4MB
__attribute__((aligned(4096)))
static uint32_t page_directory[PAGE_ENTRIES];

// 1024 page tables: 4 MB total to cover all 4 GB
__attribute__((aligned(4096)))
static uint32_t page_tables[PAGE_ENTRIES][PAGE_ENTRIES];

static inline void load_page_directory(uint32_t *pd) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(pd));
}

static inline void enable_paging(void) {
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging (PG bit)
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

void init_paging(void) {
    for (uint32_t pd_index = 0; pd_index < PAGE_ENTRIES; pd_index++) {
        for (uint32_t pt_index = 0; pt_index < PAGE_ENTRIES; pt_index++) {
            // Virtual = Physical = (pd_index * 4MB) + (pt_index * 4KB)
            uint32_t physical_addr = (pd_index << 22) | (pt_index << 12);
            page_tables[pd_index][pt_index] = physical_addr | PAGE_PRESENT | PAGE_RW;
        }

        // Point directory entry to this page table
        page_directory[pd_index] = ((uint32_t)page_tables[pd_index]) | PAGE_PRESENT | PAGE_RW;
    }

    load_page_directory(page_directory);
    enable_paging();
}

