#include "pmm.h"
#include "kprintf.h"

static uint8_t* bitmap = NULL;
static size_t bitmap_size = 0; // in bytes
static size_t total_pages = 0;
static size_t free_pages = 0;

static void bitmap_set(size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear(size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test(size_t bit) {
    return bitmap[bit / 8] & (1 << (bit % 8));
}

void pmm_init(struct limine_memmap_response* memmap) {
    uint64_t highest_address = 0;

    // 1. Find the highest physical memory address to determine bitmap size
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > highest_address) {
                highest_address = top;
            }
        }
    }

    total_pages = highest_address / PAGE_SIZE;
    bitmap_size = total_pages / 8;
    if (total_pages % 8 != 0) bitmap_size++;

    // 2. Find a place to put the bitmap itself
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        // We look for a usable chunk that is large enough.
        // We should also make sure it's below 4GB just to be safe, because Limine 
        // guarantees identity mapping below 4GB by default.
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size && entry->base < 0x100000000) {
            bitmap = (uint8_t*)entry->base;
            break;
        }
    }

    if (bitmap == NULL) {
        kprintf("PANIC: Could not find memory for PMM bitmap under 4GB!\n");
        for(;;) __asm__("hlt");
    }

    // 3. Mark ALL memory as USED by default (1)
    for (size_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0xFF;
    }

    // 4. Mark usable memory as FREE (0)
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE) {
                bitmap_clear((entry->base + j) / PAGE_SIZE);
                free_pages++;
            }
        }
    }

    // 5. Mark the memory occupied by the bitmap ITSELF as USED
    size_t bitmap_pages = bitmap_size / PAGE_SIZE;
    if (bitmap_size % PAGE_SIZE != 0) bitmap_pages++;
    for (size_t i = 0; i < bitmap_pages; i++) {
        bitmap_set(((uint64_t)bitmap / PAGE_SIZE) + i);
        free_pages--;
    }

    kprintf("PMM Initialized. Total RAM: %d MB. Free RAM: %d MB.\n", 
            (total_pages * PAGE_SIZE) / (1024 * 1024), 
            (free_pages * PAGE_SIZE) / (1024 * 1024));
}

void* pmm_alloc() {
    for (size_t bit = 0; bit < total_pages; bit++) {
        if (bitmap_test(bit) == 0) {
            // Found a free page!
            bitmap_set(bit);
            free_pages--;
            return (void*)(bit * PAGE_SIZE);
        }
    }
    kprintf("PANIC: Out of physical memory!\n");
    for(;;) __asm__("hlt");
    return NULL;
}

void pmm_free(void* ptr) {
    size_t bit = (uint64_t)ptr / PAGE_SIZE;
    if (bitmap_test(bit)) {
        bitmap_clear(bit);
        free_pages++;
    }
}
