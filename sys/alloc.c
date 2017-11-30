#include <sys/kprintf.h>
#include <sys/paging.h>
uint64_t cur_kern_heap = PAGING_VIDEO + 0x10000;

union header
{
    struct
    {
        union header* next;
        unsigned int size;
    } meta;
    long double x;
};

typedef union header Header;

Header start;
Header* freep = NULL;

void
update_pagetable(uint64_t v_addr)
{
    uint64_t p_addr = (uint64_t)paging_pagelist_get_frame();
    paging_add_pagetable_mapping(v_addr, p_addr);
}

void*
alloc_get_page()
{
    uint64_t* v_addr = (uint64_t*)cur_kern_heap;
    // kprintf("new page %p\n", v_addr);
    update_pagetable(cur_kern_heap);
    cur_kern_heap += PAGING_PAGE_SIZE;
    return (void*)v_addr;
}

void
alloc_free_page(void* v_addr)
{
    // TODO Set page table entry to not present
}

void*
pls_giv_mem(int num_bytes)
{
    static void* cur_page_va = NULL;
    static int cur_page_offset = 0;
    int num_of_pages;
    void* va_addr;

    if (cur_page_va == NULL) {
        cur_page_va = alloc_get_page();
    }

    va_addr = cur_page_va + cur_page_offset;
    if (num_bytes > (PAGING_PAGE_SIZE - cur_page_offset)) {
        num_bytes = num_bytes - (PAGING_PAGE_SIZE - cur_page_offset);
        num_of_pages = ((num_bytes - 1) / PAGING_PAGE_SIZE) + 1;

        while (num_of_pages--) {
            cur_page_va = alloc_get_page();
        }
        cur_page_offset += num_bytes % PAGING_PAGE_SIZE;
    } else {
        cur_page_offset += num_bytes;
    }
    return va_addr;
}

void
kfree(void* new)
{
    Header* temp = freep->meta.next;
    Header* new_block;
    if (new == NULL) {
        return;
    }
    new_block = (Header*)new - 1;

    while (temp->meta.next != freep)
        temp = temp->meta.next;
    temp->meta.next = new_block;
    new_block->meta.next = freep;

    // new_block->meta.next = freep;
    // freep = new_block;
}

Header*
get_mem(unsigned num_units)
{
    Header* new;
    new = (Header*)pls_giv_mem(num_units * sizeof(Header));
    new->meta.size = num_units;
    kfree((void*)(new + 1));
    return freep;
}

void*
kmalloc(size_t num_bytes)
{
    unsigned num_units;
    Header *cur, *prev;
    if (freep == NULL) {
        start.meta.next = freep = prev = &start;
        start.meta.size = 0;
    }
    num_units = ((num_bytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
    prev = freep;
    cur = prev->meta.next;
    while (1) {
        if (cur->meta.size >= num_units) {
            prev->meta.next = cur->meta.next;
            return (void*)(cur + 1);
        }
        if (cur == freep) {
            cur = get_mem(num_units);
        }
        prev = cur;
        cur = cur->meta.next;
    }
}
