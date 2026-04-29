#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

#define PAGE_SIZE 4096

void pmm_init(struct limine_memmap_response* memmap);
void* pmm_alloc();
void pmm_free(void* ptr);

#endif
