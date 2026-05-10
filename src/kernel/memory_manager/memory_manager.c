#include "memory_manager.h"
#include "../../libs/types.h"

#define ALIGN 8
#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define HEAP_SIZE (1024 * 1024)

unsigned char KHEAP[HEAP_SIZE];

typedef struct block_header {
    unsigned int size;
    unsigned int free;
    struct block_header* next;
} block_header_t;

static block_header_t* heap_head = 0;

void* kmalloc(unsigned int size){
    if (size == 0) return NULL;
    size = ALIGN_UP(size, ALIGN);

    if (!heap_head) {
        heap_head = (block_header_t*)KHEAP;
        heap_head->size = HEAP_SIZE - sizeof(block_header_t);
        heap_head->free = 1;
        heap_head->next = 0;
    }

    block_header_t* cur = heap_head;
    while (cur) {
        if (cur->free && cur->size >= size) {
            unsigned int leftover = cur->size - size;
            if (leftover > sizeof(block_header_t) + ALIGN) {
                block_header_t* blk = (block_header_t*)((unsigned char*)cur + sizeof(block_header_t) + size);
                blk->size = leftover - sizeof(block_header_t);
                blk->free = 1;
                blk->next = cur->next;
                cur->next = blk;
                cur->size = size;
            }
            cur->free = 0;
            return (void*)((unsigned char*)cur + sizeof(block_header_t));
        }
        cur = cur->next;
    }

    return NULL;
}

void kfree(void* ptr){
    if (!ptr) return;

    block_header_t* blk = (block_header_t*)((unsigned char*)ptr - sizeof(block_header_t));
    blk->free = 1;

    block_header_t* cur = heap_head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += sizeof(block_header_t) + cur->next->size;
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}