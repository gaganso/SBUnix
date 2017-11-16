#ifndef _ALLOC_H
#define _ALLOC_H
#include <sys/defs.h>
void* kmalloc(size_t size);
void* kfree(void* pt);
void* alloc_get_page();
void alloc_free_page(void* v_addr);

#endif
