#ifndef NUN_MEMORY_H
#define NUN_MEMORY_H

#include <stddef.h>
#include <stdint.h>

#define NUN_PAGE_SIZE 4096u

int nun_memory_init(uintptr_t multiboot2_info);
uint64_t nun_memory_usable_bytes(void);
void *nun_memory_allocate_frame(void);

#endif
